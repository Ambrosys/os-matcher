/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/Routing/Generic/Helper.h>
#include <AppComponents/Common/Filter/Routing/Helper.h>
#include <AppComponents/Common/Filter/Routing/SkipRouter.h>

#include <Core/Common/Geometry/Helper.h>
#include <Core/Common/Time/Helper.h>  // TODO: only for debugging

#include <amblog/global.h>

#include <mutex>
#include <thread>
#include <unordered_set>

namespace AppComponents::Common::Filter::Routing {

struct SkipRouter::Session
{
    explicit Session(
        size_t const sourceSamplingPointIndexStart_,
        size_t const targetSamplingPointIndexGoal_,
        Types::Routing::RouteList & routeList_,
        Types::Routing::RoutingStatistic & routingStatistic_)
      : sourceSamplingPointIndexStart(sourceSamplingPointIndexStart_), targetSamplingPointIndexGoal(targetSamplingPointIndexGoal_), routeList(routeList_),
        routingStatistic(routingStatistic_)
    {
    }

    size_t const sourceSamplingPointIndexStart;
    size_t const targetSamplingPointIndexGoal;
    Types::Routing::RouteList & routeList;
    Types::Routing::RoutingStatistic & routingStatistic;

    SamplingPointRouter::RouteMap routeMap;
    std::unordered_set<size_t> skippedSamplingPoints;
};

std::tuple<RouteResult, size_t> SkipRouter::routeProcess(size_t const sourceSamplingPoint, Session & session) const
{
    auto unskippedTargetSamplingPointGoal = Generic::findPreviousAllowed(session.targetSamplingPointIndexGoal + 1, sourceSamplingPoint + 1, session.skippedSamplingPoints);

    if (!unskippedTargetSamplingPointGoal)
        return {RouteResult::goalNotReachable, sourceSamplingPoint};

    if (sourceSamplingPoint >= unskippedTargetSamplingPointGoal)
        return {RouteResult::goalReached, sourceSamplingPoint};

    router_(
        session.sourceSamplingPointIndexStart,
        sourceSamplingPoint,
        *unskippedTargetSamplingPointGoal,
        session.skippedSamplingPoints,
        session.routeMap,
        session.routeList,
        session.routingStatistic);

    if (!session.routeList.empty())
    {
        size_t reachedSamplingPoint = session.routeList.back()->target.samplingPoint.index;

        if (reachedSamplingPoint >= unskippedTargetSamplingPointGoal)
            return {RouteResult::goalReached, reachedSamplingPoint};

        return {RouteResult::goalNotReachedButReachable, reachedSamplingPoint};
    }

    return {RouteResult::goalNotReachedButReachable, sourceSamplingPoint};
}

std::tuple<RouteResult, size_t> SkipRouter::skipProcess(size_t const sourceSamplingPoint, Session & session) const
{
    // TODO: only for debugging
    auto getTime = [&](size_t const samplingPoint) { return Core::Common::Time::toString(timeList_[samplingPointList_[samplingPoint].trackIndex], "%H:%M:%S"); };

    Generic::Skipper::CostFunction distanceFunction
        = [&](size_t source, size_t target) { return calcApproximateDistanceBetweenSamplingPoints(source, target, samplingPointList_); };

    auto nextUnskippedSamplingPoint = Generic::findNextAllowed(sourceSamplingPoint, session.targetSamplingPointIndexGoal, session.skippedSamplingPoints);
    assert(nextUnskippedSamplingPoint);  // should find something, because if the goal would not be reachable, skipProcess would not have been called

    Generic::SelectiveSkipper skipper{
        sourceSamplingPoint,
        *nextUnskippedSamplingPoint,
        session.sourceSamplingPointIndexStart,
        session.targetSamplingPointIndexGoal,
        session.skippedSamplingPoints,
        distanceFunction,
        configuration_.maxBacktrackingDistance,
        configuration_.skipStrategy};

    auto const routeListSnapshot = session.routeList;
    auto const alreadySkippedSampingPoints = session.skippedSamplingPoints;

    while (skipper.next())
    {
        APP_LOG(noise) << "next skip trial " << getTime(skipper.source()) << ".." << getTime(skipper.target());

        auto const routeSource = attachToPreviousRoute(session.routeList, skipper.source());

        session.skippedSamplingPoints = alreadySkippedSampingPoints;
        session.skippedSamplingPoints.merge(skipper.skipped());
        auto const [result, reached] = routeProcess(routeSource, session);

        if (result != RouteResult::goalNotReachedButReachable)
            return {result, reached};

        if (reached > sourceSamplingPoint)
        {
            APP_LOG(debug) << "successfully skipped " << getTime(skipper.source()) << ".." << getTime(skipper.target());
            return {result, reached};
        }
    }

    APP_LOG(debug) << "skipping not successful, farthest " << getTime(sourceSamplingPoint) << ", last skip " << getTime(skipper.source()) << ".." << getTime(skipper.target());
    session.routeList = routeListSnapshot;
    session.skippedSamplingPoints = alreadySkippedSampingPoints;
    return {RouteResult::goalNotReachedButReachable, sourceSamplingPoint};
}

void SkipRouter::operator()(
    size_t const sourceSamplingPointIndexStart,
    size_t const targetSamplingPointIndexGoal,
    Types::Routing::RouteList & routeList,
    Types::Routing::RoutingStatistic & routingStatistic) const
{
    APP_LOG_LOCATION("SkipRouter");

    // TODO: only for debugging
    auto getTime = [&](size_t const samplingPoint) { return Core::Common::Time::toString(timeList_[samplingPointList_[samplingPoint].trackIndex], "%H:%M:%S"); };

    Session session(sourceSamplingPointIndexStart, targetSamplingPointIndexGoal, routeList, routingStatistic);

    size_t reachedSamplingPoint = sourceSamplingPointIndexStart;

    while (true)
    {
        {
            auto const [result, reached] = routeProcess(reachedSamplingPoint, session);
            APP_LOG(debug) << "routed (normal) " << getTime(reachedSamplingPoint) << " -> " << (reached == reachedSamplingPoint ? "no advance" : getTime(reached)) << " ("
                           << routeResultToString(result) << ")";

            if (result != RouteResult::goalNotReachedButReachable)
                break;

            reachedSamplingPoint = reached;
        }

        {
            auto const [result, reached] = skipProcess(reachedSamplingPoint, session);
            APP_LOG(debug) << "routed (skipping) " << getTime(reachedSamplingPoint) << " -> " << (reached == reachedSamplingPoint ? "no advance" : getTime(reached)) << " ("
                           << routeResultToString(result) << ")";

            if (reached == reachedSamplingPoint)
                break;

            if (result != RouteResult::goalNotReachedButReachable)
                break;

            reachedSamplingPoint = reached;
        }
    }
}

}  // namespace AppComponents::Common::Filter::Routing

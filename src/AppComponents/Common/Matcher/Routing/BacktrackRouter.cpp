/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/Routing/BacktrackRouter.h>

//#include <AppComponents/Common/Filter/Routing/SamplingPointRouter.h>
#include <AppComponents/Common/Matcher/Routing/Generic/Helper.h>
#include <AppComponents/Common/Matcher/Routing/Helper.h>

#include <Core/Common/Geometry/Helper.h>
#include <Core/Common/Time/Helper.h>  // TODO: only for debugging

#include <amblog/global.h>

#include <mutex>
#include <thread>
#include <unordered_set>

namespace AppComponents::Common::Matcher::Routing {

struct BacktrackRouter::Session
{
    explicit Session(
        size_t const sourceSamplingPointIndexMinimum_,
        size_t const targetSamplingPointIndexGoal_,
        std::unordered_set<size_t> const & skippedSamplingPoints_,
        Types::Routing::RouteList & routeList_,
        SamplingPointRouter::RouteMap & routeMap_,
        Types::Routing::RoutingStatistic & routingStatistic_)
      : sourceSamplingPointIndexMinimum(sourceSamplingPointIndexMinimum_), targetSamplingPointIndexGoal(targetSamplingPointIndexGoal_),
        skippedSamplingPoints(skippedSamplingPoints_), routeList(routeList_), routingStatistic(routingStatistic_), routeMap(routeMap_)
    {
    }

    size_t const sourceSamplingPointIndexMinimum;
    size_t const targetSamplingPointIndexGoal;
    std::unordered_set<size_t> const & skippedSamplingPoints;
    Types::Routing::RouteList & routeList;
    Types::Routing::RoutingStatistic & routingStatistic;
    SamplingPointRouter::RouteMap & routeMap;

    SamplingPointRouter::VisitedRouteSet visitedRouteSet;
};

std::tuple<RouteResult, size_t> BacktrackRouter::routeProcess(size_t const sourceSamplingPoint, Session & session) const
{
    size_t reachedSamplingPoint = sourceSamplingPoint;

    while (true)
    {
        auto unskippedTargetSamplingPoint = Generic::findNextAllowed(reachedSamplingPoint, session.targetSamplingPointIndexGoal, session.skippedSamplingPoints);

        if (!unskippedTargetSamplingPoint)
            return {RouteResult::goalNotReachable, reachedSamplingPoint};

        if (reachedSamplingPoint >= session.targetSamplingPointIndexGoal)
            return {RouteResult::goalReached, reachedSamplingPoint};

        auto route = router_(reachedSamplingPoint, *unskippedTargetSamplingPoint, session.routeList, session.visitedRouteSet, session.routeMap, session.routingStatistic);

        if (route)
        {
            session.routeList.push_back(route);
            reachedSamplingPoint = route->target.samplingPoint.index;
            if (reachedSamplingPoint >= session.targetSamplingPointIndexGoal)
                return {RouteResult::goalReached, reachedSamplingPoint};
        }
        else
            return {RouteResult::goalNotReachedButReachable, reachedSamplingPoint};
    }
}

std::tuple<RouteResult, size_t> BacktrackRouter::backtrackProcess(size_t const sourceSamplingPoint, Session & session) const
{
    auto backtrackDepthTestFunction = [&](size_t const newSourceSamplingPointIndex)
    {
        if (newSourceSamplingPointIndex < session.sourceSamplingPointIndexMinimum)
            return false;
        return calcApproximateDistanceBetweenSamplingPoints(newSourceSamplingPointIndex, sourceSamplingPoint, samplingPointList_) <= configuration_.maxBacktrackingDistance;
    };

    auto const routListSnapshot = session.routeList;

    std::optional<size_t> reachedSamplingPoint = sourceSamplingPoint;

    while (true)
    {
        reachedSamplingPoint = Generic::findPreviousAllowed(*reachedSamplingPoint, session.sourceSamplingPointIndexMinimum, session.skippedSamplingPoints);
        if (!reachedSamplingPoint || !backtrackDepthTestFunction(*reachedSamplingPoint))
        {
            session.routeList = routListSnapshot;
            return {RouteResult::goalNotReachedButReachable, sourceSamplingPoint};
        }

        auto const routeSource = attachToPreviousRoute(session.routeList, *reachedSamplingPoint);
        assert(routeSource <= reachedSamplingPoint);  // May be "less than" if the previous route ended because of skipped points.

        auto const [result, reached] = routeProcess(*reachedSamplingPoint, session);

        if (result != RouteResult::goalNotReachedButReachable)
            return {result, reached};

        if (reached > sourceSamplingPoint)
            return {result, reached};

        reachedSamplingPoint = reached;
    }
}

void BacktrackRouter::operator()(
    size_t const sourceSamplingPointIndexMinimum,
    size_t const sourceSamplingPointIndexStart,
    size_t const targetSamplingPointIndexGoal,
    std::unordered_set<size_t> const & skippedSamplingPoints,
    SamplingPointRouter::RouteMap & routeMap,
    Types::Routing::RouteList & routeList,
    Types::Routing::RoutingStatistic & routingStatistic) const
{
    APP_LOG_LOCATION("BacktrackRouter");

    // TODO: only for debugging
    auto getTime = [&](size_t const samplingPoint) { return Core::Common::Time::toString(timeList_[samplingPointList_[samplingPoint].trackIndex], "%H:%M:%S"); };

    Session session(sourceSamplingPointIndexMinimum, targetSamplingPointIndexGoal, skippedSamplingPoints, routeList, routeMap, routingStatistic);

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
            auto const [result, reached] = backtrackProcess(reachedSamplingPoint, session);
            APP_LOG(debug) << "routed (backtracking) " << getTime(reachedSamplingPoint) << " -> " << (reached == reachedSamplingPoint ? "no advance" : getTime(reached)) << " ("
                           << routeResultToString(result) << ")";

            if (reached == reachedSamplingPoint)
                break;

            if (result != RouteResult::goalNotReachedButReachable)
                break;

            reachedSamplingPoint = reached;
        }
    }
}

}  // namespace AppComponents::Common::Matcher::Routing

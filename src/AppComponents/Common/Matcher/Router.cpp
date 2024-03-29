/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/Router.h>
#include <AppComponents/Common/Matcher/Routing/BacktrackRouter.h>
#include <AppComponents/Common/Matcher/Routing/DirectedCandidateRouter.h>
#include <AppComponents/Common/Matcher/Routing/Helper.h>
#include <AppComponents/Common/Matcher/Routing/PiecewiseRouter.h>
#include <AppComponents/Common/Matcher/Routing/SamplingPointRouter.h>
#include <AppComponents/Common/Matcher/Routing/SkipRouter.h>

#include <Core/Graph/Routing/Dijkstra.h>

namespace AppComponents::Common::Matcher {

Router::Router(
    double const maxVelocityDifference,
    bool const allowSelfIntersection,
    double const maxAngularDeviation,
    double const accountTurningCircleLength,
    double const maxSamplingPointSkippingDistance,
    Routing::SamplingPointSkipStrategy const samplingPointSkipStrategy,
    double const maxCandidateBacktrackingDistance,
    double const maxClusteredRoutesLengthDifference,
    Routing::RouteClusterPreference const routeClusterPreference,
    Types::Track::TimeList const & timeList,
    Types::Track::VelocityList const & velocityList,
    Types::Street::SegmentList const & segmentList)
  : Filter("Router"), maxVelocityDifference_(maxVelocityDifference), allowSelfIntersection_(allowSelfIntersection), maxAngularDeviation_(maxAngularDeviation),
    accountTurningCircleLength_(accountTurningCircleLength), maxSamplingPointSkippingDistance_(maxSamplingPointSkippingDistance),
    samplingPointSkipStrategy_(samplingPointSkipStrategy), maxCandidateBacktrackingDistance_(maxCandidateBacktrackingDistance),
    maxClusteredRoutesLengthDifference_(maxClusteredRoutesLengthDifference), routeClusterPreference_(routeClusterPreference),
    timeList_(timeList), velocityList_(velocityList), segmentList_(segmentList)
{
    setRequirements({"SamplingPointList", "Graph", "GraphEdgeMap", "StreetIndexMap"});
    setOptionals({});
    setFulfillments({"RouteList", "RoutingStatistic"});
}

bool Router::operator()(
    Types::Routing::SamplingPointList const & samplingPointList,
    Types::Graph::Graph const & graph,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Graph::StreetIndexMap const & streetIndexMap,
    Types::Routing::RouteList & routeList,
    Types::Routing::RoutingStatistic & routingStatistic)
{
    auto algorithm = Core::Graph::Routing::Dijkstra{graph};
    auto costFunction = [&](Core::Graph::Edge edge) { return Routing::geoDistance(segmentList_.at(graphEdgeMap.at(edge).streetIndex).geometry); };
    algorithm.setCost(costFunction);

    auto directedCandidateRouter = Routing::DirectedCandidateRouter{
        algorithm,
        {maxVelocityDifference_, allowSelfIntersection_, maxAngularDeviation_, accountTurningCircleLength_},
        samplingPointList,
        graphEdgeMap,
        streetIndexMap,
        timeList_,
        velocityList_,
        segmentList_};

    auto samplingPointRouter
        = Routing::SamplingPointRouter{directedCandidateRouter, {maxClusteredRoutesLengthDifference_, routeClusterPreference_}, samplingPointList, graphEdgeMap};

    auto backtrackRouter = Routing::BacktrackRouter{samplingPointRouter, {maxCandidateBacktrackingDistance_}, samplingPointList, timeList_};
    auto skipRouter = Routing::SkipRouter{backtrackRouter, {maxSamplingPointSkippingDistance_, samplingPointSkipStrategy_}, samplingPointList, timeList_};
    auto piecewiseRouter = Routing::PiecewiseRouter{skipRouter, samplingPointList, timeList_};

    piecewiseRouter(routeList, routingStatistic);

    return true;
}

}  // namespace AppComponents::Common::Matcher

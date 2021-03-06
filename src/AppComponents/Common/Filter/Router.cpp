/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/Router.h>
#include <AppComponents/Common/Filter/Routing/BacktrackRouter.h>
#include <AppComponents/Common/Filter/Routing/DirectedCandidateRouter.h>
#include <AppComponents/Common/Filter/Routing/Helper.h>
#include <AppComponents/Common/Filter/Routing/PiecewiseRouter.h>
#include <AppComponents/Common/Filter/Routing/SamplingPointRouter.h>
#include <AppComponents/Common/Filter/Routing/SkipRouter.h>

#include <Core/Graph/Routing/Dijkstra.h>

namespace AppComponents::Common::Filter {

Router::Router(
    double const maxVelocityDifference,
    bool const allowSelfIntersection,
    double const maxAngularDeviation,
    double const accountTurningCircleLength,
    double const maxSamplingPointSkippingDistance,
    Routing::SamplingPointSkipStrategy const samplingPointSkipStrategy,
    double const maxCandidateBacktrackingDistance,
    double const maxClusteredRoutesLengthDifference,
    Routing::RouteClusterPreference const routeClusterPreference)
  : Filter("Router"), maxVelocityDifference_(maxVelocityDifference), allowSelfIntersection_(allowSelfIntersection), maxAngularDeviation_(maxAngularDeviation),
    accountTurningCircleLength_(accountTurningCircleLength), maxSamplingPointSkippingDistance_(maxSamplingPointSkippingDistance),
    samplingPointSkipStrategy_(samplingPointSkipStrategy), maxCandidateBacktrackingDistance_(maxCandidateBacktrackingDistance),
    maxClusteredRoutesLengthDifference_(maxClusteredRoutesLengthDifference), routeClusterPreference_(routeClusterPreference)
{
    setRequirements({"SamplingPointList", "SegmentList", "Graph", "GraphEdgeMap", "StreetIndexMap"});
    setOptionals({"TimeList", "VelocityList"});
    setFulfillments({"RouteList", "RoutingStatistic"});
}

bool Router::operator()(
    Types::Routing::SamplingPointList const & samplingPointList,
    Types::Track::TimeList const & timeList,
    Types::Track::VelocityList const & velocityList,
    Types::Street::SegmentList const & segmentList,
    Types::Graph::Graph const & graph,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Graph::StreetIndexMap const & streetIndexMap,
    Types::Routing::RouteList & routeList,
    Types::Routing::RoutingStatistic & routingStatistic)
{
    auto algorithm = Core::Graph::Routing::Dijkstra{graph};
    auto costFunction = [&](Core::Graph::Edge edge) { return Routing::geoDistance(segmentList.at(graphEdgeMap.at(edge).streetIndex).geometry); };
    algorithm.setCost(costFunction);

    auto directedCandidateRouter = Routing::DirectedCandidateRouter{
        algorithm,
        {maxVelocityDifference_, allowSelfIntersection_, maxAngularDeviation_, accountTurningCircleLength_},
        samplingPointList,
        graphEdgeMap,
        streetIndexMap,
        timeList,
        velocityList,
        segmentList};

    auto samplingPointRouter
        = Routing::SamplingPointRouter{directedCandidateRouter, {maxClusteredRoutesLengthDifference_, routeClusterPreference_}, samplingPointList, graphEdgeMap};

    auto backtrackRouter = Routing::BacktrackRouter{samplingPointRouter, {maxCandidateBacktrackingDistance_}, samplingPointList, timeList};
    auto skipRouter = Routing::SkipRouter{backtrackRouter, {maxSamplingPointSkippingDistance_, samplingPointSkipStrategy_}, samplingPointList, timeList};
    auto piecewiseRouter = Routing::PiecewiseRouter{skipRouter, samplingPointList, timeList};

    piecewiseRouter(routeList, routingStatistic);

    return true;
}

}  // namespace AppComponents::Common::Filter

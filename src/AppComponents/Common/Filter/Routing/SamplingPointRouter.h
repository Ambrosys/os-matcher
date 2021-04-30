/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Filter/Routing/Comparators.h>
#include <AppComponents/Common/Filter/Routing/DirectedCandidateRouter.h>
#include <AppComponents/Common/Filter/Routing/Types.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>

#include <functional>
#include <memory>
#include <unordered_map>

namespace AppComponents::Common::Filter::Routing {

class SamplingPointRouter
{
public:
    using VisitedRouteSet = std::unordered_set<SamplingPointsSelection>;
    using RouteMap = std::unordered_map<SamplingPointsSelection, std::shared_ptr<Types::Routing::Route>>;

    struct Configuration
    {
        double maxClusteredRoutesLengthDifference;
        RouteClusterPreference routeClusterPreference;
    };

    /**
     * @param maxClusteredRoutesLengthDifference Should be 4 times sampling point candidate search radius.
     */
    SamplingPointRouter(
        DirectedCandidateRouter const & router,
        Configuration const configuration,
        Types::Routing::SamplingPointList const & samplingPointList,
        Types::Graph::GraphEdgeMap const & graphEdgeMap)
      : router_(router), configuration_(configuration), samplingPointList_(samplingPointList), graphEdgeMap_(graphEdgeMap)
    {
    }

    std::shared_ptr<Types::Routing::Route> operator()(
        size_t sourceSamplingPointIndex,
        size_t targetSamplingPointIndex,
        Types::Routing::RouteList const & routeList,
        VisitedRouteSet & visitedRouteSet,
        RouteMap & routeMap,
        Types::Routing::RoutingStatistic & routingStatistic) const;

private:
    std::vector<std::shared_ptr<Types::Routing::Route>> calcPossibleRoutes(
        size_t sourceSamplingPointIndex,
        size_t targetSamplingPointIndex,
        VisitedRouteSet & visitedRouteSet,
        RouteMap & routeMap,
        Types::Routing::RoutingStatistic & routingStatistic,
        Types::Routing::RouteList const & routeList) const;

    DirectedCandidateRouter const & router_;
    Configuration const configuration_;
    Types::Routing::SamplingPointList const & samplingPointList_;
    Types::Graph::GraphEdgeMap const & graphEdgeMap_;
};

}  // namespace AppComponents::Common::Filter::Routing

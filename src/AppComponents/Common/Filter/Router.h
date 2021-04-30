/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Filter/Routing/Types.h>
#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Graph/Graph.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <Core/Graph/Routing/Algorithm.h>

#include <ambpipeline/Filter.h>

namespace AppComponents::Common::Filter {

class Router : public ambpipeline::Filter
{
public:
    Router(
        double maxVelocityDifference,
        bool allowSelfIntersection,
        double maxAngularDeviation,
        double accountTurningCircleLength,
        double maxSamplingPointSkippingDistance,
        Routing::SamplingPointSkipStrategy samplingPointSkipStrategy,
        double maxCandidateBacktrackingDistance,
        double maxClusteredRoutesLengthDifference,
        Routing::RouteClusterPreference routeClusterPreference);
    bool operator()(
        Types::Routing::SamplingPointList const &,
        Types::Track::TimeList const &,
        Types::Track::VelocityList const &,
        Types::Street::SegmentList const &,
        Types::Graph::Graph const &,
        Types::Graph::GraphEdgeMap const &,
        Types::Graph::StreetIndexMap const &,
        Types::Routing::RouteList &,
        Types::Routing::RoutingStatistic &);

private:
    double const maxVelocityDifference_;
    bool const allowSelfIntersection_;
    double const maxAngularDeviation_;
    double const accountTurningCircleLength_;
    double const maxSamplingPointSkippingDistance_;
    Routing::SamplingPointSkipStrategy const samplingPointSkipStrategy_;
    double const maxCandidateBacktrackingDistance_;
    double const maxClusteredRoutesLengthDifference_;
    Routing::RouteClusterPreference const routeClusterPreference_;
};

}  // namespace AppComponents::Common::Filter

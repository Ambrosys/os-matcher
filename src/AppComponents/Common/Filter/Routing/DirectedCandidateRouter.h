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
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <Core/Graph/Routing/Algorithm.h>

#include <memory>

namespace AppComponents::Common::Filter::Routing {

class DirectedCandidateRouter
{
public:
    struct Configuration
    {
        double maxVelocityDifference;
        bool allowSelfIntersection;
        double maxAngularDeviation;
        double accountTurningCircleLength;
    };

    DirectedCandidateRouter(
        Core::Graph::Routing::RoutingAlgorithm & algorithm,
        Configuration const configuration,
        Types::Routing::SamplingPointList const & samplingPointList,
        Types::Graph::GraphEdgeMap const & graphEdgeMap,
        Types::Graph::StreetIndexMap const & streetIndexMap,
        Types::Track::TimeList const & timeList,
        Types::Track::VelocityList const & velocityList,
        Types::Street::SegmentList const & segmentList)
      : algorithm_(algorithm), configuration_(configuration), samplingPointList_(samplingPointList), graphEdgeMap_(graphEdgeMap), streetIndexMap_(streetIndexMap),
        timeList_(timeList), velocityList_(velocityList), segmentList_(segmentList)
    {
    }

    std::shared_ptr<Types::Routing::Route> operator()(SamplingPointsSelection samplingPointsSelection) const;

private:
    Core::Graph::Routing::RoutingAlgorithm & algorithm_;  // TODO: why does this compile without beeing const? operator()() is const and calls a non-const function on algorithm_.
    Configuration const configuration_;
    Types::Routing::SamplingPointList const & samplingPointList_;
    Types::Graph::GraphEdgeMap const & graphEdgeMap_;
    Types::Graph::StreetIndexMap const & streetIndexMap_;
    Types::Track::TimeList const & timeList_;
    Types::Track::VelocityList const & velocityList_;
    Types::Street::SegmentList const & segmentList_;
};

}  // namespace AppComponents::Common::Filter::Routing

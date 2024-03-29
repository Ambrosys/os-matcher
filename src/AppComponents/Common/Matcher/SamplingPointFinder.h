/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>

#include <ambpipeline/Filter.h>

namespace AppComponents::Common::Matcher {

class SamplingPointFinder : public ambpipeline::Filter
{
public:
    enum class SelectionStrategy { all, best, singles };
    SamplingPointFinder(
        SelectionStrategy selectionStrategy,
        double searchRadius,
        double maxHeadingDifference,
        Types::Track::PointList const & pointList,
        Types::Track::HeadingList const & headingList,
        Types::Street::SegmentList const & segmentList,
        Types::Street::TravelDirectionList const & travelDirectionList);
    bool operator()( Types::Routing::SamplingPointList & );

private:
    SelectionStrategy const selectionStrategy_;
    double const searchRadius_;
    double const maxHeadingDifference_;
    Types::Track::PointList const & pointList_;
    Types::Street::SegmentList const & segmentList_;
    Types::Street::TravelDirectionList const & travelDirectionList_;
    Types::Track::HeadingList const & headingList_;
};

}  // namespace AppComponents::Common::Matcher

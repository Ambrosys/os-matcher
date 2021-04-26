#pragma once

#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>

#include <ambpipeline/Filter.h>

namespace AppComponents::Common::Filter {

class SamplingPointFinder : public ambpipeline::Filter
{
public:
    enum class SelectionStrategy { all, best, singles };
    SamplingPointFinder(SelectionStrategy selectionStrategy, double searchRadius, double maxHeadingDifference);
    bool operator()(
        Types::Track::PointList const &,
        Types::Track::HeadingList const &,
        Types::Street::SegmentList const &,
        Types::Street::TravelDirectionList const &,
        Types::Routing::SamplingPointList &);

private:
    SelectionStrategy const selectionStrategy_;
    double const searchRadius_;
    double const maxHeadingDifference_;
};

}  // namespace AppComponents::Common::Filter

#pragma once

#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Filter {

class GeoJsonMapWriter : public ambpipeline::Filter
{
public:
    GeoJsonMapWriter(std::ostream & output);
    bool operator()(Types::Street::SegmentList const &, Types::Street::NodePairList const &, Types::Street::TravelDirectionList const &, Types::Street::HighwayList const &);

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Filter

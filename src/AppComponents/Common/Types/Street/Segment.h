
#pragma once

#include <Core/Common/Geometry/Types.h>

#include <vector>

namespace AppComponents::Common::Types::Street {

struct Segment
{
    size_t originId;
    size_t originOffset{0};  ///< In case the original segment was splitted.
    Core::Common::Geometry::LineString geometry;
};

using SegmentList = std::vector<Segment>;

}  // namespace AppComponents::Common::Types::Street

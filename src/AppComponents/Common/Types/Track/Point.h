
#pragma once

#include <Core/Common/Geometry/Types.h>

#include <vector>

namespace AppComponents::Common::Types::Track {

using Point = Core::Common::Geometry::Point;

struct PointList : public std::vector<Point>
{
};

}  // namespace AppComponents::Common::Types::Track

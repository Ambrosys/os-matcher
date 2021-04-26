
#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

/// height above ground
using Altitude = double;

struct AltitudeList : public std::vector<Altitude>
{
};

}  // namespace AppComponents::Common::Types::Track

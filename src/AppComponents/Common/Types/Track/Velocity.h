
#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

using Velocity = double;

struct VelocityList : public std::vector<Velocity>
{
};

}  // namespace AppComponents::Common::Types::Track

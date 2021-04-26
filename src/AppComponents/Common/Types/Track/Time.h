
#pragma once

#include <chrono>
#include <vector>

namespace AppComponents::Common::Types::Track {

using Time = std::chrono::system_clock::time_point;

struct TimeList : public std::vector<Time>
{
};

}  // namespace AppComponents::Common::Types::Track

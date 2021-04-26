
#pragma once

#include <vector>

namespace AppComponents::Common::Types::Street {

enum class TravelDirection { both, forwards, backwards };

using TravelDirectionList = std::vector<TravelDirection>;

}  // namespace AppComponents::Common::Types::Street

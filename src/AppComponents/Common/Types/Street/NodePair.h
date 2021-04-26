
#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace AppComponents::Common::Types::Street {

using NodePair = std::pair<size_t, size_t>;

struct NodePairList : public std::vector<NodePair>
{
};

}  // namespace AppComponents::Common::Types::Street

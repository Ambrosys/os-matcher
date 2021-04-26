
#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

using Heading = double;

struct HeadingList : public std::vector<Heading>
{
};

}  // namespace AppComponents::Common::Types::Track

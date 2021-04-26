#include <AppComponents/Common/Filter/Routing/Generic/Helper.h>

namespace AppComponents::Common::Filter::Routing::Generic {

std::optional<size_t> findPreviousAllowed(size_t value, size_t min, std::unordered_set<size_t> const & blocklist)
{
    size_t i = value;
    while (i == value || blocklist.find(i) != blocklist.end())
        if (i > min)
            i--;
        else
            return std::nullopt;
    return i;
}

std::optional<size_t> findNextAllowed(size_t value, size_t max, std::unordered_set<size_t> const & blocklist)
{
    size_t i = value;
    while (i == value || blocklist.find(i) != blocklist.end())
        if (i < max)
            i++;
        else
            return std::nullopt;
    return i;
}

}  // namespace AppComponents::Common::Filter::Routing::Generic

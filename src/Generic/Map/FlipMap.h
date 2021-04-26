#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>

// https://stackoverflow.com/a/5056797

namespace {

template <typename A, typename B>
std::pair<B, A> flipPair(const std::pair<A, B> & p)
{
    return std::pair<B, A>(p.second, p.first);
}

}  // namespace

namespace Generic {

// flips an associative container of A,B pairs to B,A pairs
template <typename A, typename B, typename C = std::less<B>, template <class, class, class...> class M, class... Args>
std::multimap<B, A, C> flipMap(const M<A, B, Args...> & src, C const & cmp = C())
{
    std::multimap<B, A, C> dst(cmp);
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), flipPair<A, B>);
    return dst;
}

}  // namespace Generic
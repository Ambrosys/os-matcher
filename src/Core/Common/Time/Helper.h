#pragma once

#include <chrono>
#include <string>
#include <utility>

namespace Core::Common::Time {

std::string toString(std::chrono::system_clock::time_point time, std::string const & format);

/**
 * @return Sth. like `2018-06-22T03:14:10[.000]`.
 */
inline std::string toIsoString(std::chrono::system_clock::time_point time)
{
    return toString(time, "%Y-%m-%dT%H:%M:%S");
}

/**
 * @return Sth. like `2018-06-22T03:14:10[.000]Z`.
 */
inline std::string toIsoZString(std::chrono::system_clock::time_point time)
{
    return toString(time, "%Y-%m-%dT%H:%M:%SZ");
}

std::pair<std::chrono::system_clock::time_point, bool> fromString(std::string const & text, std::string const & format);

/**
 *
 * @param text  Sth. like `2018-06-22T03:14:10[.000]`.
 */
inline std::pair<std::chrono::system_clock::time_point, bool> fromIsoString(std::string const & text)
{
    return fromString(text, "%Y-%m-%dT%H:%M:%S");
}

/**
 *
 * @param text  Sth. like `2018-06-22T03:14:10[.000]Z`.
 */
inline std::pair<std::chrono::system_clock::time_point, bool> fromIsoZString(std::string const & text)
{
    return fromString(text, "%Y-%m-%dT%H:%M:%SZ");
}

}  // namespace Core::Common::Time

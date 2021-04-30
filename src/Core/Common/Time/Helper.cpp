/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Common/Time/Helper.h>

#include <ctime>
#include <iomanip>
#include <sstream>

namespace Core::Common::Time {

std::string toString(std::chrono::system_clock::time_point time, std::string const & format)
{
    std::time_t time_t = std::chrono::system_clock::to_time_t(time);
    std::ostringstream ss;
    //ss.imbue( std::locale( "de_DE.utf-8" ) );
    std::tm tm{};
    ss << std::put_time(localtime_r(&time_t, &tm), format.c_str());
    return ss.str();
}

std::pair<std::chrono::system_clock::time_point, bool> fromString(std::string const & text, std::string const & format)
{
    std::tm tm{};
    std::istringstream ss(text);
    //ss.imbue( std::locale( "de_DE.utf-8" ) );
    ss >> std::get_time(&tm, format.c_str());
    if (ss.fail())
        return {{}, false};
    tm.tm_isdst = -1;  // TODO: check how to do it right
    return {std::chrono::system_clock::from_time_t(std::mktime(&tm)), true};
}

}  // namespace Core::Common::Time

/*
 * SPDX-FileCopyrightText: © 2017 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace Generic::String {

// https://stackoverflow.com/a/217605

// trim from start (in place)
static inline void ltrim(std::string & s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string & s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string & io_s_)
{
    ltrim(io_s_);
    rtrim(io_s_);
}

// trim from start (copying)
inline std::string ltrimmed(std::string s_)
{
    ltrim(s_);
    return s_;
}

// trim from end (copying)
inline std::string rtrimmed(std::string s_)
{
    rtrim(s_);
    return s_;
}

// trim from both ends (copying)
inline std::string trimmed(std::string s_)
{
    trim(s_);
    return s_;
}

}  // namespace Generic::String

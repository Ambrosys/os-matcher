
/*
 * Author: Fabian Sandoval Saldias (fabianvss@gmail.com)
 * Date: 2017-04-03
 */

#pragma once

#include <sstream>
#include <string>

namespace Generic::String {

// https://stackoverflow.com/a/236803

template <typename Out>
void split(std::string const & s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

}  // namespace Generic::String

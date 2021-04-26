
/*
 * Author: Fabian Sandoval Saldias (fabianvss@gmail.com)
 * Date: 2017-04-03
 */

#pragma once

#include <Generic/String/Trim.h>

#include <sstream>
#include <string>
#include <vector>

namespace Generic::String {

std::vector<std::string> readNextRow(std::istream & str, char const separator)
{
    std::vector<std::string> data;
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    while (std::getline(lineStream, cell, separator))
    {
        Generic::String::trim(cell);
        data.push_back(cell);
    }
    if (!lineStream && cell.empty())  // trailing comma
        data.emplace_back();
    return data;
}

}  // namespace Generic::String

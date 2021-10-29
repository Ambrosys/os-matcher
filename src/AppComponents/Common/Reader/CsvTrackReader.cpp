/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Reader/CsvTrackReader.h>

#include <Core/Common/Time/Helper.h>

#include <Generic/String/Csv.h>

#include <amblog/global.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <locale>

namespace AppComponents::Common::Reader {

CsvTrackReader::CsvTrackReader(std::istream & input) : input_(input)
{
}

bool CsvTrackReader::operator()(
    Common::Types::Track::TimeList & timeList,
    Common::Types::Track::PointList & pointList,
    Common::Types::Track::HeadingList & headingList,
    Common::Types::Track::VelocityList & velocityList)
{
    using namespace Common;

    APP_LOG_TAG(noise, "I/O") << "Reading track";

    while (true)
    {
        auto data = Generic::String::readNextRow(input_, ';');
        if (data.empty())
            break;
        if (data.size() <= 4)
            break;

        auto try_time = Core::Common::Time::fromIsoString(data[0]);
        if (!try_time.second)
        {
            APP_LOG(error) << "Invalid date/time format: " << data[0];
            APP_THROW_LOGGED_EXCEPTION();
        }

        timeList.emplace_back(try_time.first);
        pointList.emplace_back(Types::Track::Point::Latitude{std::stod(data[1])}, Types::Track::Point::Longitude{std::stod(data[2])});
        headingList.emplace_back(std::stod(data[3]));
        velocityList.emplace_back(std::stod(data[4]));
    }

    return true;
}

}  // namespace AppComponents::Common::Reader

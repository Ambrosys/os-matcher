/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/JsonTrackReader.h>

#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>
#include <nlohmann/json.hpp>

namespace AppComponents::Common::Filter {

JsonTrackReader::JsonTrackReader(std::istream & input) : Filter("JsonTrackReader"), input_(input)
{
    setRequirements({});
    setOptionals({});
    setFulfillments({"TimeList", "PointList", "PartialHeadingList", "VelocityList"});
}

bool JsonTrackReader::operator()(
    Common::Types::Track::TimeList & timeList,
    Common::Types::Track::PointList & pointList,
    Common::Types::Track::HeadingList & headingList,
    Common::Types::Track::VelocityList & velocityList)
{
    using namespace Common;

    APP_LOG_TAG(noise, "I/O") << "Reading track";

    nlohmann::json json;
    input_ >> json;

    for (auto const & data : json)
    {
        auto [time, valid] = Core::Common::Time::fromIsoZString(data.at("pos_time"));
        if (!valid)
        {
            APP_LOG(error) << "Invalid date/time format: " << data.at("pos_time");
            APP_THROW_LOGGED_EXCEPTION();
        }

        timeList.emplace_back(time);
        pointList.emplace_back(
            Types::Track::Point::Latitude{data.at("latitude").get<Core::Common::Geometry::ValueType>()},
            Types::Track::Point::Longitude{data.at("longitude").get<Core::Common::Geometry::ValueType>()});
        if (data.find("course") != data.end())
            headingList.emplace_back(data.at("course").get<Types::Track::Heading>());
        else
            headingList.emplace_back(NAN);
        velocityList.emplace_back(data.at("speed").get<Types::Track::Velocity>() / 3.6);
    }

    return true;
}

}  // namespace AppComponents::Common::Filter

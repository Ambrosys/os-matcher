/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Writer/GeoJsonTrackWriter.h>

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>
#include <nlohmann/json.hpp>

namespace AppComponents::Common::Writer {

GeoJsonTrackWriter::GeoJsonTrackWriter(std::ostream & output) : Filter("GeoJsonTrackWriter"), output_(output)
{
    setRequirements({"TimeList", "PointList", "VelocityList"});
    setOptionals({"HeadingList", "PartialHeadingList"});
    setFulfillments({"track written"});
}

bool GeoJsonTrackWriter::operator()(
    Types::Track::TimeList const & timeList,
    Types::Track::PointList const & pointList,
    Types::Track::HeadingList const & headingList,
    Types::Track::VelocityList const & velocityList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing track";

    output_ << R"RAW({ "type": "FeatureCollection", "features": [)RAW" << '\n';

    for (size_t i = 0; i < pointList.size(); ++i)
    {
        auto const & point = pointList[i];

        auto properties = nlohmann::json{{"time", Core::Common::Time::toIsoString(timeList.at(i))}, {"velocity", velocityList.at(i)}};

        if (not headingList.empty() and not std::isnan(headingList.at(i)))
            properties.push_back({"heading", headingList.at(i)});

        output_ << nlohmann::json{{"type", "Feature"}, {"geometry", Core::Common::Geometry::toGeoJson(point)}, {"properties", properties}};

        if (i + 1 < pointList.size())
            output_ << ',';
        output_ << '\n';
    }

    output_ << "] }";

    return true;
}

}  // namespace AppComponents::Common::Writer

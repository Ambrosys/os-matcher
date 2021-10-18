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

GeoJsonTrackWriter::GeoJsonTrackWriter(std::ostream & output) : output_(output)
{}

bool GeoJsonTrackWriter::operator()(
    std::ostream & output,
    Types::Track::TimeList const & timeList,
    Types::Track::PointList const & pointList,
    Types::Track::HeadingList const & headingList,
    Types::Track::VelocityList const & velocityList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing track";

    output << R"RAW({ "type": "FeatureCollection", "features": [)RAW" << '\n';

    for (size_t i = 0; i < pointList.size(); ++i)
    {
        auto const & point = pointList[i];

        auto properties = nlohmann::json{{"time", Core::Common::Time::toIsoString(timeList.at(i))}, {"velocity", velocityList.at(i)}};

        if (not headingList.empty() and not std::isnan(headingList.at(i)))
            properties.push_back({"heading", headingList.at(i)});

        output << nlohmann::json{{"type", "Feature"}, {"geometry", Core::Common::Geometry::toGeoJson(point)}, {"properties", properties}};

        if (i + 1 < pointList.size())
            output << ',';
        output << '\n';
    }

    output << "] }";

    return true;
}

bool GeoJsonTrackWriter::operator()(
    Types::Track::TimeList const & timeList,
    Types::Track::PointList const & pointList,
    Types::Track::HeadingList const & headingList,
    Types::Track::VelocityList const & velocityList)
{
    return this->operator()(
        output_,
        timeList,
        pointList,
        headingList,
        velocityList
        );
}

}  // namespace AppComponents::Common::Writer

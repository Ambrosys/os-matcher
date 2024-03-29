/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Reader/GeoJsonMapReader.h>

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Geometry/Helper.h>

#include <amblog/global.h>
#include <nlohmann/json.hpp>

namespace AppComponents::Common::Reader {

namespace {

    Types::Street::TravelDirection toTravelDirection(std::string const & text)
    {
        if (text == "Both")
            return Types::Street::TravelDirection::both;
        if (text == "Forwards")
            return Types::Street::TravelDirection::forwards;
        if (text == "Backwards")
            return Types::Street::TravelDirection::backwards;
        APP_LOG(error) << "TravelDirection '" << text << "' unknown";
        APP_THROW_LOGGED_EXCEPTION();
    }

    Types::Street::Highway toHighway(std::string const & text)
    {
        using Types::Street::HighwayType;
        if (text == "Motorway")
            return HighwayType::motorway;
        if (text == "Trunk")
            return HighwayType::trunk;
        if (text == "Primary")
            return HighwayType::primary;
        if (text == "Secondary")
            return HighwayType::secondary;
        if (text == "Tertiary")
            return HighwayType::tertiary;
        if (text == "MotorwayLink")
            return HighwayType::motorway_link;
        if (text == "TrunkLink")
            return HighwayType::trunk_link;
        if (text == "PrimaryLink")
            return HighwayType::primary_link;
        if (text == "SecondaryLink")
            return HighwayType::secondary_link;
        if (text == "TertiaryLink")
            return HighwayType::tertiary_link;
        if (text == "Unknown")
            return std::nullopt;
        APP_LOG(error) << "Highway '" << text << "' unknown";
        APP_THROW_LOGGED_EXCEPTION();
    }

}  // namespace

GeoJsonMapReader::GeoJsonMapReader(std::istream & input) : input_(input)
{
}

bool GeoJsonMapReader::operator()(
    Types::Street::SegmentList & segmentList,
    Types::Street::NodePairList & nodePairList,
    Types::Street::TravelDirectionList & travelDirectionList,
    Types::Street::HighwayList & highwayList)
{
    APP_LOG_TAG(noise, "I/O") << "Reading map";

    nlohmann::json json;
    input_ >> json;

    if (json.at("type").get<std::string>() != "FeatureCollection")
    {
        APP_LOG(error) << "'type' of JSON data has to be 'FeatureCollection' but is '" << json.at("type").get<std::string>() << '\'';
        APP_THROW_LOGGED_EXCEPTION();
    }

    nlohmann::json & features = json.at("features");
    for (auto const & feature : features)
    {
        if (feature.at("type").get<std::string>() != "Feature")
        {
            APP_LOG(error) << "'type' of FeatureCollection item has to be 'Feature' but is '" << feature.at("type").get<std::string>() << '\'';
            APP_THROW_LOGGED_EXCEPTION();
        }

        auto const & properties = feature.at("properties");

        segmentList.push_back({properties.at("Id").get<size_t>(), properties.at("Offset").get<size_t>(), Core::Common::Geometry::toLineString(feature.at("geometry"))});
        nodePairList.push_back({properties.at("SourceNode").get<size_t>(), properties.at("TargetNode").get<size_t>()});
        travelDirectionList.push_back(toTravelDirection(properties.at("TravelDirection").get<std::string>()));
        highwayList.push_back(toHighway(properties.at("Highway").get<std::string>()));
    }

    APP_LOG_MS(noise) << segmentList.size() << " street segments created";

    return true;
}

}  // namespace AppComponents::Common::Reader

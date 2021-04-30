/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/GeoJsonMapWriter.h>

#include <Core/Common/Geometry/Conversion.h>

#include <amblog/global.h>
#include <nlohmann/json.hpp>

#include <cassert>
#include <string>

namespace AppComponents::Common::Filter {

namespace {

    std::string toString(Types::Street::TravelDirection const travelDirection)
    {
        switch (travelDirection)
        {
            case Types::Street::TravelDirection::both: return "Both";
            case Types::Street::TravelDirection::forwards: return "Forwards";
            case Types::Street::TravelDirection::backwards: return "Backwards";
        }
        assert(false);
        return {};
    }

    std::string toString(Types::Street::Highway const highway)
    {
        if (!highway)
            return "Unknown";
        using Types::Street::HighwayType;
        switch (*highway)
        {
            case HighwayType::motorway: return "Motorway";
            case HighwayType::trunk: return "Trunk";
            case HighwayType::primary: return "Primary";
            case HighwayType::secondary: return "Secondary";
            case HighwayType::tertiary: return "Tertiary";
            case HighwayType::motorway_link: return "MotorwayLink";
            case HighwayType::trunk_link: return "TrunkLink";
            case HighwayType::primary_link: return "PrimaryLink";
            case HighwayType::secondary_link: return "SecondaryLink";
            case HighwayType::tertiary_link: return "TertiaryLink";
        }
        assert(false);
        return {};
    }

}  // namespace

GeoJsonMapWriter::GeoJsonMapWriter(std::ostream & output) : Filter("GeoJsonMapWriter"), output_(output)
{
    setRequirements({"SegmentList", "NodePairList", "TravelDirectionList", "HighwayList"});
    setOptionals({});
    setFulfillments({"map written"});
}

bool GeoJsonMapWriter::operator()(
    Types::Street::SegmentList const & segmentList,
    Types::Street::NodePairList const & nodePairList,
    Types::Street::TravelDirectionList const & travelDirectionList,
    Types::Street::HighwayList const & highwayList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing map";

    assert(segmentList.size() == nodePairList.size());
    assert(segmentList.size() == travelDirectionList.size());

    output_ << R"RAW({ "type": "FeatureCollection", "features": [)RAW" << '\n';

    //auto features = nlohmann::json::array();
    for (size_t i = 0; i < segmentList.size(); ++i)
    {
        auto const & segment = segmentList[i];
        auto const & nodePair = nodePairList[i];
        auto const & travelDirection = travelDirectionList[i];
        auto const & highway = highwayList[i];

        //features.push_back( nlohmann::json{
        output_ << nlohmann::json{
            {"type", "Feature"},
            {"geometry", Core::Common::Geometry::toGeoJson(segment.geometry)},
            {"properties",
             nlohmann::json{
                 {"Id", segment.originId},
                 {"Offset", segment.originOffset},
                 {"SourceNode", nodePair.first},
                 {"TargetNode", nodePair.second},
                 {"TravelDirection", toString(travelDirection)},
                 {"Highway", toString(highway)}}}};
        if (i + 1 < segmentList.size())
            output_ << ',';
        output_ << '\n';
    }

    //nlohmann::json json{
    //    { "type", "FeatureCollection" },
    //    { "features", features }
    //};

    //output_ << json.dump( 2 );

    output_ << "] }";

    return true;
}

}  // namespace AppComponents::Common::Filter

#include <AppComponents/Common/Filter/Osm/Conversion.h>

#include <cassert>
#include <sstream>

namespace AppComponents::Common::Filter::Osm {

Types::Street::TravelDirection toTravelDirection(std::string const & oneway)
{
    /*
     * Occurences germany 2018-07-03:
        12075333 null
          555758 "yes"
          139965 "no"
            1944 "-1"
              50 "reversible"
              18 "ccw"
              17 "alternating"
               9 "forward"
               6 "1"
               4 "recommended"
               3 "unknown"
               2 "use_sidepath"
               2 "fixme"
               1 "-1;no"
               1 "yes/no"
               1 "undefined"
     */

    using Types::Street::TravelDirection;

    // most frequent occurences:
    if (oneway.empty() || oneway == "no")
        return TravelDirection::both;
    if (oneway == "yes")
        return TravelDirection::forwards;
    if (oneway == "-1")
        return TravelDirection::backwards;

    // occurences below 0.007 percent:
    if (oneway == "forward" || oneway == "1")
        return TravelDirection::forwards;
    if (oneway == "-1;no")
        return TravelDirection::backwards;

    return TravelDirection::both;
}

Types::Street::Highway toHighway(std::string const & highway)
{
    using Types::Street::HighwayType;
    if (highway == "motorway")
        return HighwayType::motorway;
    if (highway == "trunk")
        return HighwayType::trunk;
    if (highway == "primary")
        return HighwayType::primary;
    if (highway == "secondary")
        return HighwayType::secondary;
    if (highway == "tertiary")
        return HighwayType::tertiary;
    if (highway == "motorway_link")
        return HighwayType::motorway_link;
    if (highway == "trunk_link")
        return HighwayType::trunk_link;
    if (highway == "primary_link")
        return HighwayType::primary_link;
    if (highway == "secondary_link")
        return HighwayType::secondary_link;
    if (highway == "tertiary_link")
        return HighwayType::tertiary_link;
    return std::nullopt;
}

std::string toOsmString(Types::Street::HighwayType const highway)
{
    using Types::Street::HighwayType;
    switch (highway)
    {
        case HighwayType::motorway: return "motorway";
        case HighwayType::trunk: return "trunk";
        case HighwayType::primary: return "primary";
        case HighwayType::secondary: return "secondary";
        case HighwayType::tertiary: return "tertiary";
        case HighwayType::motorway_link: return "motorway_link";
        case HighwayType::trunk_link: return "trunk_link";
        case HighwayType::primary_link: return "primary_link";
        case HighwayType::secondary_link: return "secondary_link";
        case HighwayType::tertiary_link: return "tertiary_link";
    }
    assert(false);
    return {};
}

std::string toHighwaySelectionSql(std::unordered_set<Types::Street::HighwayType> const & highways, std::string const & tableName)
{
    std::stringstream ss;
    for (auto it = highways.begin(); it != highways.end(); ++it)
    {
        if (it != highways.begin())
            ss << " or ";
        ss << tableName << ".highway='" << toOsmString(*it) << '\'';
    }
    return ss.str();
}

}  // namespace AppComponents::Common::Filter::Osm

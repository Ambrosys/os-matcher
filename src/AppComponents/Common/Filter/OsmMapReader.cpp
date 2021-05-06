/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/Osm/Conversion.h>
#include <AppComponents/Common/Filter/OsmMapReader.h>

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Geometry/Helper.h>
#include <Core/Common/Postgres/Helper.h>

#include <Generic/String/Split.h>

#include <amblog/global.h>
#include <pqxx/pqxx>

#include <boost/algorithm/string/replace.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <iomanip>
#include <iterator>
#include <unordered_map>
#include <vector>

namespace AppComponents::Common::Filter {

namespace {

    struct PointLocation
    {
        size_t osmLineId;
        size_t pointIndex;
    };

    struct UniquePoint
    {
        size_t id;
        std::vector<PointLocation> locations;
    };

    struct OsmPointCandidate
    {
        size_t id;
        Core::Common::Geometry::Point point;
        //std::string layer;
        //std::string level;
        //std::string location;
    };

    struct OsmLineCandidate
    {
        Types::Street::Segment segment;
        Types::Street::TravelDirection travelDirection;
        Types::Street::Highway highway;
        //std::string layer;
        //std::string level;
        //std::string location;
    };

    struct Candidate
    {
        OsmLineCandidate segment;
        std::vector<size_t> osmPointIds;
    };

    using PointGeoindexValue = std::pair<Core::Common::Geometry::Point, std::shared_ptr<UniquePoint>>;
    using PointGeoindexAlgorithm = boost::geometry::index::quadratic<16>;
    using PointGeoindex = boost::geometry::index::rtree<PointGeoindexValue, PointGeoindexAlgorithm>;

    /**
     * Points are considered equal if the distance is 10cm or less.
     */
    std::shared_ptr<UniquePoint> getUniquePoint(PointGeoindex & geoindex, Core::Common::Geometry::Point point)
    {
        unsigned const k_nearest = 1;
        double const maxDistanceInMeters = 0.1;
        auto result = std::optional<PointGeoindexValue>{};
        auto inserter = [&](PointGeoindexValue const & value)
        {
            if (geoDistance(value.first, point) <= maxDistanceInMeters)
                result = value;
        };
        geoindex.query(boost::geometry::index::nearest(point, k_nearest), boost::make_function_output_iterator(inserter));
        if (not result)
        {
            result = PointGeoindexValue{point, std::make_shared<UniquePoint>()};
            geoindex.insert(*result);
        }
        return result->second;
    }

    /**
     * @param records The query should have returned lines and points pairwise sequentially (multiply line data),
     *   like { { line1, point1 }, { line1, point2 }, { line2, point1 }, ... }.
     * @return { candidates, osmPointMap, geoindex }
     */
    std::tuple<std::vector<Candidate>, std::unordered_map<size_t, OsmPointCandidate>, PointGeoindex> getCandidates(pqxx::result & records)
    {
        using namespace Core::Common::Geometry;
        using namespace Core::Common::Postgres;
        using namespace AppComponents::Common::Filter::Osm;

        auto candidates = std::vector<Candidate>{};
        auto osmPointMap = std::unordered_map<size_t, OsmPointCandidate>{};
        auto geoindex = PointGeoindex{};

        auto currentSegment = std::optional<OsmLineCandidate>{};
        auto currentNodes = std::vector<size_t>{};

        size_t nextPointId = 0;
        auto addCurrentStreet = [&]()
        {
            assert(currentSegment);
            for (size_t i = 0; i < currentSegment->segment.geometry.size(); ++i)
            {
                auto & uniquePoint = *getUniquePoint(geoindex, currentSegment->segment.geometry[i]);
                uniquePoint.id = nextPointId++;  // TODO: only increment if a new UniquePoint was created?
                uniquePoint.locations.push_back(PointLocation{currentSegment->segment.originId, i});
            }
            candidates.push_back(Candidate{std::move(*currentSegment), std::move(currentNodes)});
            currentSegment.reset();
            currentNodes.clear();
        };

        for (auto const & row : records)
        {
            auto segmentIdRaw = row.at("line_id").as<int64_t>();
            size_t segmentId = segmentIdRaw >= 0 ? static_cast<size_t>(segmentIdRaw) : static_cast<size_t>(-segmentIdRaw);
            if (not currentSegment || currentSegment->segment.originId != segmentId)
            {
                if (currentSegment)
                    addCurrentStreet();
                auto way = row.at("line_way").as<std::string>();
                auto oneway = getOptional(row.at("line_oneway"), std::string{});
                currentSegment = OsmLineCandidate{
                    {segmentId, 0, toLineString(way)}, toTravelDirection(oneway), toHighway(row.at("line_highway").as<std::string>())
                    //getOptional( row.at( "line_layer" ), std::string{} ),
                    //getOptional( row.at( "line_level" ), std::string{} ),
                    //getOptional( row.at( "line_location" ), std::string{} )
                };
            }
            /*
            auto osmPointId = getOptional<size_t>( row.at( "point_id" ) );
            if( osmPointId )
            {
                if( osmPointMap.find( *osmPointId ) == osmPointMap.end() )
                {
                    auto pointway = getOptional<std::string>( row.at( "point_way" ) );
                    if( !pointway )
                    {
                        APP_LOG( error ) << "planet_osm_point.way is null where planet_osm_point.osm_id is not";
                        APP_THROW_LOGGED_EXCEPTION();
                    }

                    osmPointMap.emplace( *osmPointId, OsmPointCandidate{
                        *osmPointId,
                        toPoint( *pointway )
                        //getOptional( row.at( "point_layer" ), std::string{} ),
                        //getOptional( row.at( "point_level" ), std::string{} ),
                        //getOptional( row.at( "point_location" ), std::string{} )
                    } );
                }
                currentNodes.emplace_back( *osmPointId );
            }
            */
        }
        if (currentSegment)
            addCurrentStreet();

        return {std::move(candidates), std::move(osmPointMap), std::move(geoindex)};
    }

    /**
     * Note: Candidates are split on shared point intersections.
     */
    std::tuple<Types::Street::SegmentList, Types::Street::NodePairList, Types::Street::TravelDirectionList, Types::Street::HighwayList>
    processCandidates(std::vector<Candidate> const & candidates, std::unordered_map<size_t, OsmPointCandidate> const & osmPointMap [[gnu::unused]], PointGeoindex & geoindex)
    {
        Types::Street::SegmentList segmentList;
        Types::Street::NodePairList nodePairList;
        Types::Street::TravelDirectionList travelDirectionList;
        Types::Street::HighwayList highwayList;

        auto addStreet = [&](OsmLineCandidate segmentCandidate, size_t firstPointIndex, size_t lastPointIndex, size_t firstPointId, size_t lastPointId)
        {
            Types::Street::Segment segment;
            segment.originId = segmentCandidate.segment.originId;
            segment.originOffset = firstPointIndex;
            for (size_t i = firstPointIndex; i <= lastPointIndex; ++i)
                segment.geometry.push_back(segmentCandidate.segment.geometry[i]);
            segmentList.push_back(segment);
            nodePairList.emplace_back(firstPointId, lastPointId);
            travelDirectionList.push_back(segmentCandidate.travelDirection);
            highwayList.push_back(segmentCandidate.highway);
        };

        for (auto const & candidate : candidates)
        {
            size_t firstPointIndex = 0;
            size_t firstPointId = getUniquePoint(geoindex, candidate.segment.segment.geometry[firstPointIndex])->id;
            for (size_t i = 1; i < candidate.segment.segment.geometry.size() - 1; ++i)
            {
                auto & uniquePoint = *getUniquePoint(geoindex, candidate.segment.segment.geometry[i]);
                if (uniquePoint.locations.size() > 1)  // split street on junction
                {
                    addStreet(candidate.segment, firstPointIndex, i, firstPointId, uniquePoint.id);
                    firstPointIndex = i;
                    firstPointId = uniquePoint.id;
                }
            }
            size_t lastPointIndex = candidate.segment.segment.geometry.size() - 1;
            size_t lastPointId = getUniquePoint(geoindex, candidate.segment.segment.geometry[lastPointIndex])->id;
            addStreet(candidate.segment, firstPointIndex, lastPointIndex, firstPointId, lastPointId);
        }

        return {std::move(segmentList), std::move(nodePairList), std::move(travelDirectionList), std::move(highwayList)};
    }

}  // namespace

OsmMapReader::OsmMapReader(
    Core::Common::Postgres::Connection & connection,
    std::unordered_set<Types::Street::HighwayType> const & highwaySelection,
    double const fetchCorridor,
    bool const useSingleSearchCircle)
  : Filter("OsmMapReader"), connection_(connection), highwaySelection_(highwaySelection), fetchCorridor_(fetchCorridor), useSingleSearchCircle_(useSingleSearchCircle)
{
    setRequirements({"PointList"});
    setOptionals({});
    setFulfillments({"SegmentList", "NodePairList", "TravelDirectionList", "HighwayList"});
}

bool OsmMapReader::operator()(
    Types::Track::PointList const & pointList,
    Types::Street::SegmentList & segmentList,
    Types::Street::NodePairList & nodePairList,
    Types::Street::TravelDirectionList & travelDirectionList,
    Types::Street::HighwayList & highwayList)
{
    using namespace Core::Common;
    using namespace AppComponents::Common::Filter::Osm;

    size_t const eachNth = 1;

    APP_LOG_TAG_MS(noise, "DB") << "Sending each " << eachNth << "th of " << pointList.size() << " points to database...";

    double searchRadius;
    std::string pointsString;
    if (!useSingleSearchCircle_)
    {
        searchRadius = fetchCorridor_;
        pointsString = Geometry::toWkt(Geometry::flattened_simple(pointList, eachNth));
    }
    else
    {
        if (pointList.empty())
        {
            searchRadius = 0;
            pointsString = Geometry::toWkt(std::vector<Geometry::Point>{});
        }
        else if (pointList.size() == 1)
        {
            searchRadius = fetchCorridor_;
            pointsString = toWkt(pointList[0]);
        }
        else
        {
            double distance = Geometry::geoDistance(pointList[0], pointList[pointList.size() - 1]);
            double heading = Geometry::heading(pointList[0], pointList[pointList.size() - 1]);
            auto center = Geometry::reverseHaversine(pointList[0], heading, distance / 2.0);

            searchRadius = distance / 2.0 + fetchCorridor_;
            pointsString = toWkt(center);
        }
    }

    auto dbConnection = connection_.getConnection();
    auto dbTransaction = pqxx::work{*dbConnection};

    // Occurences germany 2018-07-18:
    //   count highway
    // 2624300 track
    // 2059463 service
    // 1694021 residential
    // 1287753 footway
    //  996774 path
    //  363015 secondary
    //  339168 unclassified
    //  329683 tertiary
    //  184903 steps
    //  181721 primary
    //  123117 living_street
    //  106666 cycleway
    //   62585 motorway
    //   38529 motorway_link
    //   28494 trunk
    //   26718 pedestrian
    //   18351 trunk_link
    //   17649 primary_link
    //    9541 secondary_link
    //    8937 construction
    //    8931 platform
    //    6581 bridleway
    //    6264 proposed
    //    4243 tertiary_link
    //    3104 road
    //    1929 corridor
    //    1443 raceway
    //     920 elevator
    //     739 bus_stop
    //     424 rest_area
    //    1460 [53 different types]

    /* // TODO: This is the query with layer data from lines and points.
    auto query = std::string{ R"sql(
        select
            line.osm_id     line_id,
            line.oneway     line_oneway,
            line.layer      line_layer,
            line.level      line_level,
            line.location   line_location,
            point.osm_id    point_id,
            point.layer     point_layer,
            point.level     point_level,
            point.location  point_location,
            ST_AsText( ST_Transform( line.way, 4326 ) )   line_way,
            ST_AsText( ST_Transform( point.way, 4326 ) )  point_way
        from
            planet_osm_line   line
        left outer join
            planet_osm_point  point
            on
                ST_Touches( line.way, point.way )
                and (point.layer is not null or point.level is not null or point.location is not null)
        where
            (
                line.highway = 'motorway'
                or line.highway = 'trunk'
                or line.highway = 'primary'
                or line.highway = 'secondary'
                or line.highway = 'motorway_link'
                or line.highway = 'trunk_link'
                or line.highway = 'primary_link'
                or line.highway = 'secondary_link'
            )
            and ST_DWithin( ST_Transform( ST_GeomFromText( $1, 4326 ), 32632 ), line.way, $2 )
        )sql" };
    */
    auto query = std::string{R"sql(
        select
            line.osm_id  line_id,
            line.oneway  line_oneway,
            line.highway line_highway,
            ST_AsText( ST_Transform( line.way, 4326 ) )  line_way
        from
            planet_osm_line  line
        where
            ( $HIGHWAY_CONDITION )
            and ST_DWithin( ST_Transform( ST_GeomFromText( $1, 4326 ), 32632 ), line.way, $2 )
        )sql"};
    boost::replace_all(query, "$HIGHWAY_CONDITION", toHighwaySelectionSql(highwaySelection_, "line"));
    dbConnection->prepare("wayquery", query);
    pqxx::result records = dbTransaction.exec_prepared("wayquery", pointsString, searchRadius);

    APP_LOG_TAG_MS(noise, "DB") << records.size() << " records were read";

    auto [candidates, osmPointMap, geoindex] = getCandidates(records);

    APP_LOG_TAG_MS(noise, "DB") << candidates.size() << " lines, " << osmPointMap.size() << " points and " << geoindex.size() << " coordinates fetched";

    std::tie(segmentList, nodePairList, travelDirectionList, highwayList) = processCandidates(candidates, osmPointMap, geoindex);

    APP_LOG_MS(noise) << segmentList.size() << " street segments created";

    return true;
}

}  // namespace AppComponents::Common::Filter

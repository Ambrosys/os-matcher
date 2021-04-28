
#include "Context.h"
#include "Filter/CsvRouteWriter.h"
#include "Filter/CsvSubRouteWriter.h"
#include "Filter/GeoJsonRouteWriter.h"

#include <AppComponents/Common/Filter/GeoJsonMapReader.h>
#include <AppComponents/Common/Filter/GeoJsonMapWriter.h>
#include <AppComponents/Common/Filter/GraphBuilder.h>
#include <AppComponents/Common/Filter/JsonRouteStatisticWriter.h>
#include <AppComponents/Common/Filter/OsmMapReader.h>
#include <AppComponents/Common/Filter/Router.h>
#include <AppComponents/Common/Filter/SamplingPointFinder.h>
#include <AppComponents/ExampleMatcher/Filter/CsvTrackReader.h>
#include <AppComponents/ExampleMatcher/Filter/JsonTrackReader.h>
#include <OsMatcher/OsMatcherVersion.h>

#include <Core/Common/Postgres/Connection.h>

#include <ambpipeline/Filter.h>
#include <ambpipeline/Pipeline.h>
#include <cliapp/CliApp.h>

#include <experimental/filesystem>

#include <fstream>
#include <memory>
#include <unordered_set>

namespace {

using namespace AppComponents::Common;

//@{
/// Filter signature definitions
using TrackIoFilter = std::function<bool(Types::Track::TimeList &, Types::Track::PointList &, Types::Track::HeadingList &, Types::Track::VelocityList &)>;
using MapReaderWriterFilter = std::function<bool(Types::Street::SegmentList &, Types::Street::NodePairList &, Types::Street::TravelDirectionList &, Types::Street::HighwayList &)>;
using PartialMapReaderFilter = std::function<bool(
    Types::Track::PointList &, Types::Street::SegmentList &, Types::Street::NodePairList &, Types::Street::TravelDirectionList &, Types::Street::HighwayList &)>;
using RouterFilter = std::function<bool(
    Types::Routing::SamplingPointList &,
    Types::Track::TimeList &,
    Types::Track::VelocityList &,
    Types::Street::SegmentList &,
    Types::Graph::Graph &,
    Types::Graph::GraphEdgeMap &,
    Types::Graph::StreetIndexMap &,
    Types::Routing::RouteList &,
    Types::Routing::RoutingStatistic &)>;
using GraphBuilderFilter = std::function<bool(
    Types::Street::NodePairList &,
    Types::Street::TravelDirectionList &,
    Types::Graph::Graph &,
    Types::Graph::GraphEdgeMap &,
    Types::Graph::StreetIndexMap &,
    Types::Graph::NodeMap &)>;
using SamplingPointFinderFilter = std::function<bool(
    Types::Track::PointList &, Types::Track::HeadingList &, Types::Street::SegmentList &, Types::Street::TravelDirectionList &, Types::Routing::SamplingPointList &)>;
using SamplingPointWriterFilter = std::function<bool(Types::Routing::SamplingPointList &, Types::Track::TimeList &, Types::Track::PointList &, Types::Street::SegmentList &)>;
using StreetRouteWriterFilter = std::function<bool(
    Types::Routing::RouteList &,
    Types::Graph::GraphEdgeMap &,
    Types::Graph::NodeMap &,
    Types::Track::TimeList &,
    Types::Street::SegmentList &,
    Types::Routing::SamplingPointList &)>;
using RouteStatisticWriter = std::function<bool(Types::Routing::RoutingStatistic &, Types::Routing::SamplingPointList &, Types::Track::TimeList &)>;
//@}
using Feature = std::function<bool()>;

struct FilterVisitor
{
    FilterVisitor(Context & context) : c(context) {}

    //@{
    /// Filter calls
    bool operator()(TrackIoFilter & filter) { return filter(c.track.timeList, c.track.pointList, c.track.headingList, c.track.velocityList); };
    bool operator()(MapReaderWriterFilter & filter) { return filter(c.street.segmentList, c.street.nodePairList, c.street.travelDirectionList, c.street.highwayList); };
    bool operator()(PartialMapReaderFilter & filter)
    {
        return filter(c.track.pointList, c.street.segmentList, c.street.nodePairList, c.street.travelDirectionList, c.street.highwayList);
    };
    bool operator()(RouterFilter & filter)
    {
        return filter(
            c.routing.samplingPointList,
            c.track.timeList,
            c.track.velocityList,
            c.street.segmentList,
            c.graph.lemonDigraph,
            c.graph.graphEdgeMap,
            c.graph.streetIndexMap,
            c.routing.routeList,
            c.routing.routingStatistic);
    };
    bool operator()(GraphBuilderFilter & filter)
    {
        return filter(c.street.nodePairList, c.street.travelDirectionList, c.graph.lemonDigraph, c.graph.graphEdgeMap, c.graph.streetIndexMap, c.graph.nodeMap);
    };
    bool operator()(SamplingPointFinderFilter & filter)
    {
        return filter(c.track.pointList, c.track.headingList, c.street.segmentList, c.street.travelDirectionList, c.routing.samplingPointList);
    };
    bool operator()(SamplingPointWriterFilter & filter) { return filter(c.routing.samplingPointList, c.track.timeList, c.track.pointList, c.street.segmentList); };
    bool operator()(StreetRouteWriterFilter & filter)
    {
        return filter(c.routing.routeList, c.graph.graphEdgeMap, c.graph.nodeMap, c.track.timeList, c.street.segmentList, c.routing.samplingPointList);
    };
    bool operator()(RouteStatisticWriter & filter) { return filter(c.routing.routingStatistic, c.routing.samplingPointList, c.track.timeList); };
    //@}
    bool operator()(ambpipeline::DummyFilterFunction & filter) { return filter({}); };
    bool operator()(Feature & filter) { return filter(); };

private:
    Context & c;
};

using Pipeline = ambpipeline::Pipeline<
    //@{
    /// Filter signature registrations
    TrackIoFilter,
    MapReaderWriterFilter,
    PartialMapReaderFilter,
    RouterFilter,
    GraphBuilderFilter,
    SamplingPointFinderFilter,
    SamplingPointWriterFilter,
    StreetRouteWriterFilter,
    RouteStatisticWriter,
    //@}
    ambpipeline::DummyFilterFunction,
    Feature>;

struct UserOptions : public cliapp::UserOptionsBase
{
    std::string fspIn;
    std::string mapSource{"auto"};
    std::string mapIn;
    std::string mapOut;
    std::string routeCsvOut;
    std::string subRouteCsvOut;
    std::string routeGeoJsonOut;
    std::string routeStatisticJsonOut;
    std::string pipelineOut;
    std::string dbHost{"localhost"};
    std::string dbUser{"docker"};
    std::string dbName{"gis"};
    std::string dbPass{"docker"};
    unsigned short dbPort{5432};
};

int app(UserOptions options)
{
    //@{
    /// Necessary due to immatureness of Clara
    if (options.fspIn.empty())
    {
        APP_LOG(fatal) << "Error in command line:\n--fsp-in is required";
        return EXIT_FAILURE;
    }
    //@}

    if (not options.mapIn.empty() and options.mapSource != "auto")
    {
        APP_LOG(fatal) << "Error in command line:\n--map-source has to be \"auto\" when specifying --map-in";
        return EXIT_FAILURE;
    }

    if (not std::experimental::filesystem::exists(options.fspIn))
    {
        APP_LOG(fatal) << "Fsp file does not exist: " << options.fspIn;
        return EXIT_FAILURE;
    }

    if (not options.mapIn.empty() and not std::experimental::filesystem::exists(options.mapIn))
    {
        APP_LOG(fatal) << "Map file does not exist: " << options.mapIn;
        return EXIT_FAILURE;
    }

    double const mapFetchCorridor = 300.0;
    double const samplingPointSearchRadius = 30.0;
    double const maxSamplingPointHeadingDifference = 90.0;
    double const maxVelocityDifference = 10.0;
    double const maxSamplingPointSkippingDistance = 3000.0;
    double const maxCandidateBacktrackingDistance = 1000.0;

    auto postgresConnection = Core::Common::Postgres::Connection{
        Core::Common::Postgres::Connection::Strategy::globalUnlocked, options.dbHost, options.dbPort, options.dbName, options.dbUser, options.dbPass};

    auto pipeline = Pipeline{};
    auto context = Context{};
    auto visitor = FilterVisitor{context};
    auto ensure = std::vector<std::string>{};

    auto fspIn = std::ifstream{options.fspIn};
    {
        auto extension = std::experimental::filesystem::path(options.fspIn).extension();
        if (extension == ".csv" || extension == ".txt")
            pipeline.add(AppComponents::ExampleMatcher::Filter::CsvTrackReader{fspIn});
        else if (extension == ".json")
            pipeline.add(AppComponents::ExampleMatcher::Filter::JsonTrackReader{fspIn});
        else
        {
            APP_LOG(fatal) << "Fsp input file extension unknown: " << extension;
            return EXIT_FAILURE;
        }
    }

    auto mapIn = std::ifstream{};
    if (options.mapIn.empty())
    {
        using Types::Street::HighwayType;
        std::unordered_set<HighwayType> highwaySelection
            = {HighwayType::motorway,
               HighwayType::trunk,
               HighwayType::primary,
               HighwayType::secondary,
               HighwayType::tertiary,
               HighwayType::motorway_link,
               HighwayType::trunk_link,
               HighwayType::primary_link,
               HighwayType::secondary_link,
               HighwayType::tertiary_link};
        if (options.mapSource == "auto")
            pipeline.add(Filter::OsmMapReader{postgresConnection, highwaySelection, mapFetchCorridor, false});
        else
        {
            APP_LOG(fatal) << "Map source unknown: " << options.mapSource << " (has to be "
                           << "\"auto\" (for osm postgres db or geojson file)" << ')';
            return EXIT_FAILURE;
        }
    }
    else
    {
        mapIn = std::ifstream{options.mapIn};
        auto extension = std::experimental::filesystem::path(options.mapIn).extension();
        if (extension == ".geojson")
            pipeline.add(AppComponents::Common::Filter::GeoJsonMapReader{mapIn});
        else
        {
            APP_LOG(fatal) << "Map input file extension unknown: " << extension;
            return EXIT_FAILURE;
        }
    }

    auto mapOut = std::unique_ptr<std::ofstream>{};
    if (!options.mapOut.empty())
    {
        mapOut = std::make_unique<std::ofstream>(options.mapOut);
        pipeline.add(Filter::GeoJsonMapWriter{*mapOut});
        ensure.emplace_back("map written");
    }

    auto routeCsvOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeCsvOut.empty())
    {
        routeCsvOut = std::make_unique<std::ofstream>(options.routeCsvOut);
        pipeline.add(Applications::ExampleMatcher::Filter::CsvRouteWriter{*routeCsvOut});
        ensure.emplace_back("CsvRouteWriter");
    }

    auto subRouteCsvOut = std::unique_ptr<std::ofstream>{};
    if (!options.subRouteCsvOut.empty())
    {
        subRouteCsvOut = std::make_unique<std::ofstream>(options.subRouteCsvOut);
        pipeline.add(Applications::ExampleMatcher::Filter::CsvSubRouteWriter{*subRouteCsvOut});
        ensure.emplace_back("CsvSubRouteWriter");
    }

    auto routeGeoJsonOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeGeoJsonOut.empty())
    {
        routeGeoJsonOut = std::make_unique<std::ofstream>(options.routeGeoJsonOut);
        pipeline.add(Applications::ExampleMatcher::Filter::GeoJsonRouteWriter{*routeGeoJsonOut});
        ensure.emplace_back("GeoJsonRouteWriter");
    }

    auto routeStatisticJsonOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeStatisticJsonOut.empty())
    {
        routeStatisticJsonOut = std::make_unique<std::ofstream>(options.routeStatisticJsonOut);
        pipeline.add(AppComponents::Common::Filter::JsonRouteStatisticWriter{*routeStatisticJsonOut});
        ensure.emplace_back("JsonRouteStatisticWriter");
    }

    pipeline.add(Filter::Router{
        maxVelocityDifference,
        true,
        360.0,
        5.0,
        maxSamplingPointSkippingDistance,
        Filter::Routing::SamplingPointSkipStrategy::excludeEdgeCosts,
        maxCandidateBacktrackingDistance,
        4.0 * samplingPointSearchRadius,
        Filter::Routing::RouteClusterPreference::shortest});
    pipeline.add(Filter::GraphBuilder{});
    pipeline.add(Filter::SamplingPointFinder{Filter::SamplingPointFinder::SelectionStrategy::all, samplingPointSearchRadius, maxSamplingPointHeadingDifference});

    auto pipelineObject = pipeline.compile(ensure);

    if (!options.pipelineOut.empty())
    {
        auto pipelineOut = std::ofstream{options.pipelineOut};
        pipelineOut << Pipeline::toDot(pipelineObject.graph, ensure);
    }

    pipelineObject.run(visitor, not options.noColor);

    return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char * argv[])
{
    UserOptions options;
    auto cli = clara::Opt(options.fspIn, "file")["--fsp-in"]("fahrspur input").required() | clara::Opt(options.mapSource, "auto")["--map-source"]("map source").optional()
             | clara::Opt(options.mapIn, "file")["--map-in"]("map input").optional() | clara::Opt(options.mapOut, "file")["--map-out"]("map output").optional()
             | clara::Opt(options.routeCsvOut, "file")["--route"]("route output").optional()
             | clara::Opt(options.subRouteCsvOut, "file")["--sub-route"]("sub route output").optional()
             | clara::Opt(options.routeGeoJsonOut, "file")["--route-geojson"]("route output").optional()
             | clara::Opt(options.routeStatisticJsonOut, "file")["--route-statistic"]("route statistic output").optional()
             | clara::Opt(options.pipelineOut, "file")["--pipeline"]("pipeline graph output (dot)").optional()
             | clara::Opt(options.dbHost, "db host")["--host"]("db host name").optional() | clara::Opt(options.dbPort, "db port")["--port"]("db host port").optional()
             | clara::Opt(options.dbName, "db name")["--db"]("database").optional() | clara::Opt(options.dbUser, "db user")["--dbuser"]("db user").optional()
             | clara::Opt(options.dbPass, "db password")["--dbpasswd"]("db password").optional();

    return cliapp::main(argc, argv, cli, options, "ExampleMatcher", "v" + std::string{OSMATCHER_VERSION_SHORT}, app);
}

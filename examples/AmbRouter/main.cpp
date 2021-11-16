
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AppComponents/Common/Writer/CsvRouteWriter.h"
#include "AppComponents/Common/Writer/CsvSubRouteWriter.h"
#include "AppComponents/Common/Writer/GeoJsonRouteWriter.h"
#include "Context.h"

#include <AppComponents/Common/Matcher/GraphBuilder.h>
#include <AppComponents/Common/Matcher/Router.h>
#include <AppComponents/Common/Matcher/SamplingPointFinder.h>
#include <AppComponents/Common/Reader/CsvTrackReader.h>
#include <AppComponents/Common/Reader/GeoJsonMapReader.h>
#include <AppComponents/Common/Reader/JsonTrackReader.h>
#include <AppComponents/Common/Reader/OsmMapReader.h>
#include <AppComponents/Common/Writer/GeoJsonMapWriter.h>
#include <AppComponents/Common/Writer/JsonRouteStatisticWriter.h>
#include <OsMatcher/OsMatcherVersion.h>

#include <Core/Common/Postgres/Connection.h>

#include <amblog/global.h>
#include <ambpipeline/Filter.h>
#include <ambpipeline/Pipeline.h>
#include <cliapp/CliApp.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_set>

namespace {

using namespace AppComponents::Common;

//@{
/// Filter signature definitions
using RouterFilter = std::function<bool(
    Types::Routing::SamplingPointList &,
    Types::Graph::Graph &,
    Types::Graph::GraphEdgeMap &,
    Types::Graph::StreetIndexMap &,
    Types::Routing::RouteList &,
    Types::Routing::RoutingStatistic &)>;
using GraphBuilderFilter = std::function<bool(Types::Graph::Graph &, Types::Graph::GraphEdgeMap &, Types::Graph::StreetIndexMap &, Types::Graph::NodeMap &)>;
using SamplingPointFinderFilter = std::function<bool(Types::Routing::SamplingPointList &)>;
//@}
using Feature = std::function<bool()>;

struct FilterVisitor
{
    FilterVisitor(Context & context) : c(context) {}

    //@{
    /// Filter calls
    bool operator()(RouterFilter & filter)
    {
        return filter(c.routing.samplingPointList, c.graph.lemonDigraph, c.graph.graphEdgeMap, c.graph.streetIndexMap, c.routing.routeList, c.routing.routingStatistic);
    };
    bool operator()(GraphBuilderFilter & filter) { return filter(c.graph.lemonDigraph, c.graph.graphEdgeMap, c.graph.streetIndexMap, c.graph.nodeMap); };
    bool operator()(SamplingPointFinderFilter & filter) { return filter(c.routing.samplingPointList); };
    //@}
    bool operator()(ambpipeline::DummyFilterFunction & filter) { return filter({}); };
    bool operator()(Feature & filter) { return filter(); };

private:
    Context & c;
};

using Pipeline = ambpipeline::Pipeline<
    //@{
    /// Filter signature registrations
    RouterFilter,
    GraphBuilderFilter,
    SamplingPointFinderFilter,
    //@}
    ambpipeline::DummyFilterFunction,
    Feature>;

struct UserOptions : public cliapp::UserOptionsBase
{
    std::string trackIn;
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
    bool noSplitStreets{false};
};

int app(UserOptions options)
{
    if (not options.mapIn.empty() and options.mapSource != "auto")
    {
        APP_LOG(fatal) << "Error in command line:\n--map-source has to be \"auto\" when specifying --map-in";
        return EXIT_FAILURE;
    }

    if (not std::filesystem::exists(options.trackIn))
    {
        APP_LOG(fatal) << "Track file does not exist: " << options.trackIn;
        return EXIT_FAILURE;
    }

    if (not options.mapIn.empty() and not std::filesystem::exists(options.mapIn))
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

    // Reader

    auto trackIn = std::ifstream{options.trackIn};
    {
        auto extension = std::filesystem::path(options.trackIn).extension();
        if (extension == ".csv" || extension == ".txt")
        {
            AppComponents::Common::Reader::CsvTrackReader{trackIn}(context.track.timeList, context.track.pointList, context.track.headingList, context.track.velocityList);
            APP_LOG(info) << "len(context.track.timeList) = " << context.track.timeList.size();
        }
        else if (extension == ".json")
            AppComponents::Common::Reader::JsonTrackReader{trackIn}(context.track.timeList, context.track.pointList, context.track.headingList, context.track.velocityList);
        else
        {
            APP_LOG(fatal) << "Track input file extension unknown: " << extension;
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
            Reader::OsmMapReader{postgresConnection, highwaySelection, mapFetchCorridor, false, !options.noSplitStreets}(
                context.track.pointList, context.street.segmentList, context.street.nodePairList, context.street.travelDirectionList, context.street.highwayList);
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
        auto extension = std::filesystem::path(options.mapIn).extension();
        if (extension == ".geojson")
            AppComponents::Common::Reader::GeoJsonMapReader{mapIn}(
                context.street.segmentList, context.street.nodePairList, context.street.travelDirectionList, context.street.highwayList);
        else
        {
            APP_LOG(fatal) << "Map input file extension unknown: " << extension;
            return EXIT_FAILURE;
        }
    }

    // TODO: Extend Reader Interface with registrable thread pool functionality to maintain multithreading while reading

    // Matcher Pipeline

    pipeline.add(Matcher::Router{
        maxVelocityDifference,
        true,
        360.0,
        5.0,
        maxSamplingPointSkippingDistance,
        Matcher::Routing::SamplingPointSkipStrategy::excludeEdgeCosts,
        maxCandidateBacktrackingDistance,
        4.0 * samplingPointSearchRadius,
        Matcher::Routing::RouteClusterPreference::shortest,
        context.track.timeList,
        context.track.velocityList,
        context.street.segmentList});
    pipeline.add(Matcher::GraphBuilder{context.street.nodePairList, context.street.travelDirectionList});
    pipeline.add(Matcher::SamplingPointFinder{
        Matcher::SamplingPointFinder::SelectionStrategy::all,
        samplingPointSearchRadius,
        maxSamplingPointHeadingDifference,
        context.track.pointList,
        context.track.headingList,
        context.street.segmentList,
        context.street.travelDirectionList});
    // Note: ensure shall not be empty so the Pipeline optimizer does not clear the whole piepline
    ensure.emplace_back("Router");

    auto pipelineObject = pipeline.compile(ensure);

    if (!options.pipelineOut.empty())
    {
        auto pipelineOut = std::ofstream{options.pipelineOut};
        pipelineOut << Pipeline::toDot(pipelineObject.graph, ensure);
    }

    pipelineObject.run(visitor, not options.noColor);

    // Writer

    // TODO: Extend Reader Interface with registrable thread pool functionality to maintain multithreading while reading
    //       see ambthread and ambpipeline lib (ambpipeline::Pipeline.tpp:283ff)

    auto mapOut = std::unique_ptr<std::ofstream>{};
    if (!options.mapOut.empty())
    {
        mapOut = std::make_unique<std::ofstream>(options.mapOut);
        Writer::GeoJsonMapWriter{*mapOut}(context.street.segmentList, context.street.nodePairList, context.street.travelDirectionList, context.street.highwayList);
        ensure.emplace_back("map written");
    }

    auto routeCsvOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeCsvOut.empty())
    {
        routeCsvOut = std::make_unique<std::ofstream>(options.routeCsvOut);
        AppComponents::Common::Writer::CsvRouteWriter{*routeCsvOut}(
            context.routing.routeList, context.graph.graphEdgeMap, context.graph.nodeMap, context.track.timeList, context.street.segmentList, context.routing.samplingPointList);
        ensure.emplace_back("CsvRouteWriter");
    }

    auto subRouteCsvOut = std::unique_ptr<std::ofstream>{};
    if (!options.subRouteCsvOut.empty())
    {
        subRouteCsvOut = std::make_unique<std::ofstream>(options.subRouteCsvOut);
        AppComponents::Common::Writer::CsvSubRouteWriter{*subRouteCsvOut}(
            context.routing.routeList, context.graph.graphEdgeMap, context.graph.nodeMap, context.track.timeList, context.street.segmentList, context.routing.samplingPointList);
        ensure.emplace_back("CsvSubRouteWriter");
    }

    auto routeGeoJsonOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeGeoJsonOut.empty())
    {
        routeGeoJsonOut = std::make_unique<std::ofstream>(options.routeGeoJsonOut);
        AppComponents::Common::Writer::GeoJsonRouteWriter{*routeGeoJsonOut}(
            context.routing.routeList, context.graph.graphEdgeMap, context.graph.nodeMap, context.track.timeList, context.street.segmentList, context.routing.samplingPointList);
        ensure.emplace_back("GeoJsonRouteWriter");
    }

    auto routeStatisticJsonOut = std::unique_ptr<std::ofstream>{};
    if (!options.routeStatisticJsonOut.empty())
    {
        routeStatisticJsonOut = std::make_unique<std::ofstream>(options.routeStatisticJsonOut);
        AppComponents::Common::Writer::JsonRouteStatisticWriter{*routeStatisticJsonOut}(
            context.routing.routingStatistic, context.routing.samplingPointList, context.track.timeList);
        ensure.emplace_back("JsonRouteStatisticWriter");
    }

    return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char * argv[])
{
    UserOptions options;
    auto cli
        = lyra::opt(options.trackIn, "file")["--track-in"]("track input").required() | lyra::opt(options.mapSource, "auto")["--map-source"]("map source").optional()
        | lyra::opt(options.mapIn, "file")["--map-in"]("map input").optional() | lyra::opt(options.mapOut, "file")["--map-out"]("map output").optional()
        | lyra::opt(options.routeCsvOut, "file")["--route"]("route output").optional() | lyra::opt(options.subRouteCsvOut, "file")["--sub-route"]("sub route output").optional()
        | lyra::opt(options.routeGeoJsonOut, "file")["--route-geojson"]("route output").optional()
        | lyra::opt(options.routeStatisticJsonOut, "file")["--route-statistic"]("route statistic output").optional()
        | lyra::opt(options.pipelineOut, "file")["--pipeline"]("pipeline graph output (dot)").optional() | lyra::opt(options.dbHost, "db host")["--host"]("db host name").optional()
        | lyra::opt(options.dbPort, "db port")["--port"]("db host port").optional() | lyra::opt(options.dbName, "db name")["--db"]("database").optional()
        | lyra::opt(options.dbUser, "db user")["--dbuser"]("db user").optional() | lyra::opt(options.dbPass, "db password")["--dbpasswd"]("db password").optional()
        | lyra::opt(options.noSplitStreets)["--no-split-streets"]("do not split streets on overlapping points").optional();

    return cliapp::main(argc, argv, cli, options, "AmbRouter", "v" + std::string{OSMATCHER_VERSION_SHORT}, "Ambrosys Router application.", app);
}

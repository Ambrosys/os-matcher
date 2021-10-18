/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "GeoJsonRouteWriter.h"

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>

#include <nlohmann/json.hpp>

namespace AppComponents::Common::Writer {

GeoJsonRouteWriter::GeoJsonRouteWriter(std::ostream & output) : output_(output)
{}

bool GeoJsonRouteWriter::operator()(
    std::ostream & output,
    Types::Routing::RouteList const & routeList,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Street::SegmentList const & segmentList,
    Types::Graph::NodeMap const & nodeMap [[gnu::unused]],
    Types::Track::TimeList const & timeList,
    Types::Routing::SamplingPointList const & samplingPointList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing route";

    output << R"RAW({ "type": "FeatureCollection", "features": [)RAW" << '\n';

    bool someRoutePrinted = false;
    for (auto const & route : routeList)
        for (auto const & subRoute : route->subRoutes)
        {
            auto streetEdge = graphEdgeMap.at(subRoute.edge);
            auto segment = segmentList[streetEdge.streetIndex];

            assert(!subRoute.route.empty());
            if (subRoute.route.size() == 1)
                continue;  // Skip routes with 1 entry.

            if (someRoutePrinted)
                output << ",\n";

            output << nlohmann::json{
                {"type", "Feature"},
                {"geometry", Core::Common::Geometry::toGeoJson(subRoute.route)},
                {"properties",
                 nlohmann::json{
                     {"Id", segment.originId},
                     {"entry_time", Core::Common::Time::toIsoZString(timeList.at(samplingPointList.at(route->source.samplingPoint.index).trackIndex))},
                     {"exit_time", Core::Common::Time::toIsoZString(timeList.at(samplingPointList.at(route->target.samplingPoint.index).trackIndex))}}}};

            someRoutePrinted = true;
        }

    if (someRoutePrinted)
        output << '\n';

    output << "] }";

    return true;
}

bool GeoJsonRouteWriter::operator()(
    Types::Routing::RouteList const & routeList,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Graph::NodeMap const & nodeMap [[gnu::unused]],
    Types::Track::TimeList const & timeList,
    Types::Street::SegmentList const & segmentList,
    Types::Routing::SamplingPointList const & samplingPointList)
{
    return this->operator()(
        output_,
        routeList,
        graphEdgeMap,
        segmentList,
        nodeMap,
        timeList,
        samplingPointList
    );
}

}  // namespace AppComponents::Common::Writer

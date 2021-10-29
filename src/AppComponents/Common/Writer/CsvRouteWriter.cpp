/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CsvRouteWriter.h"

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>

#include <algorithm>
#include <sstream>
#include <fstream>

namespace AppComponents::Common::Writer {

CsvRouteWriter::CsvRouteWriter(std::ostream & output) : output_(output)
{}

bool CsvRouteWriter::operator()(
    std::ostream & output,
    Types::Routing::RouteList const & routeList,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Street::SegmentList const & segmentList,
    Types::Graph::NodeMap const & nodeMap,
    Types::Track::TimeList const & timeList,
    Types::Routing::SamplingPointList const & samplingPointList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing routes";

    output
        << "osm_ids;route;length;cost;sourceNode;targetNode;sourceSamplingPointTime;targetSamplingPointTime;sourceSamplingPointCandidateIndex;targetSamplingPointCandidateIndex;sourceSamplingPointCandidateConsideredForwards;targetSamplingPointCandidateConsideredForwards;routeStartPoint;routeEndPoint\n";
    output << std::setprecision(14);
    for (auto const & route : routeList)
    {
        auto const & source = route->source;
        auto const & target = route->target;
        auto const & subRoutes = route->subRoutes;
        std::vector<Core::Common::Geometry::Point> points;
        std::vector<size_t> osmIds;
        double totalCost = 0.0;
        double totalLength = 0.0;
        for (auto const & [edge, cost, route, length] : subRoutes)
        {
            auto streetEdge = graphEdgeMap.at(edge);
            auto segment = segmentList[streetEdge.streetIndex];
            for (size_t i = points.empty() ? 0 : 1; i < route.size(); ++i)
                points.push_back(route[i]);
            if (osmIds.empty() || osmIds.back() != segment.originId)
                osmIds.push_back(segment.originId);
            totalCost += cost;
            totalLength += length;
        }
        {
            std::stringstream ss;
            copy(osmIds.begin(), osmIds.end(), std::ostream_iterator<size_t>(ss, ","));
            std::string str = ss.str();
            str.resize(str.length() - 1);
            output << str << ';';
        }
        output << Core::Common::Geometry::toWkt(points) << ';';
        output << totalLength << ';';
        output << totalCost << ';';
        output << nodeMap.at(source.node) << ';';
        output << nodeMap.at(target.node) << ';';
        output << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(source.samplingPoint.index).trackIndex)) << ';';
        output << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(target.samplingPoint.index).trackIndex)) << ';';
        output << source.samplingPoint.candidate.index << ';';
        output << target.samplingPoint.candidate.index << ';';
        output << source.samplingPoint.candidate.consideredForwards << ';';
        output << target.samplingPoint.candidate.consideredForwards << ';';
        output << Core::Common::Geometry::toWkt(points[0]) << ';';
        output << Core::Common::Geometry::toWkt(points[points.size() - 1]) << '\n';
    }

    return true;
}

bool CsvRouteWriter::operator()(
    Types::Routing::RouteList const & routeList,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Graph::NodeMap const & nodeMap,
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
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CsvSubRouteWriter.h"

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>

namespace AppComponents::Common::Writer {

CsvSubRouteWriter::CsvSubRouteWriter(std::ostream & output) : output_(output)
{
}

bool CsvSubRouteWriter::operator()(
    std::ostream & output,
    Types::Routing::RouteList const & routeList,
    Types::Graph::GraphEdgeMap const & graphEdgeMap,
    Types::Street::SegmentList const & segmentList,
    Types::Graph::NodeMap const & nodeMap,
    Types::Track::TimeList const & timeList,
    Types::Routing::SamplingPointList const & samplingPointList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing sub routes";

    output
        << "osm_id;route;length;cost;sourceNode;targetNode;sourceSamplingPointTime;targetSamplingPointTime;sourceSamplingPointCandidateIndex;targetSamplingPointCandidateIndex;sourceSamplingPointCandidateConsideredForwards;targetSamplingPointCandidateConsideredForwards\n";
    output << std::setprecision(14);
    for (auto const & route : routeList)
    {
        auto const & source = route->source;
        auto const & target = route->target;
        auto const & subRoutes = route->subRoutes;
        for (auto const & [edge, cost, route, length] : subRoutes)
        {
            auto streetEdge = graphEdgeMap.at(edge);
            auto segment = segmentList[streetEdge.streetIndex];
            std::vector<Core::Common::Geometry::Point> points;
            std::transform(route.begin(), route.end(), std::back_inserter(points), [](const auto point) { return point; });
            output << segment.originId << ';';
            output << Core::Common::Geometry::toWkt(points) << ';';
            output << length << ';';
            output << cost << ';';
            output << nodeMap.at(source.node) << ';';
            output << nodeMap.at(target.node) << ';';
            output << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(source.samplingPoint.index).trackIndex)) << ';';
            output << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(target.samplingPoint.index).trackIndex)) << ';';
            output << source.samplingPoint.candidate.index << ';';
            output << target.samplingPoint.candidate.index << ';';
            output << source.samplingPoint.candidate.consideredForwards << ';';
            output << target.samplingPoint.candidate.consideredForwards << '\n';
        }
    }

    return true;
}

bool CsvSubRouteWriter::operator()(
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

/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CsvSubRouteWriter.h"

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>

namespace Applications::ExampleMatcher::Filter {

CsvSubRouteWriter::CsvSubRouteWriter(std::ostream & output) : Filter("CsvSubRouteWriter"), output_(output)
{
    setRequirements({"RouteList", "GraphEdgeMap", "NodeMap", "TimeList", "SegmentList", "SamplingPointList"});
    setOptionals({});
    setFulfillments({"route written"});
}

bool CsvSubRouteWriter::operator()(
    AppComponents::Common::Types::Routing::RouteList const & routeList,
    AppComponents::Common::Types::Graph::GraphEdgeMap const & graphEdgeMap,
    AppComponents::Common::Types::Graph::NodeMap const & nodeMap,
    AppComponents::Common::Types::Track::TimeList const & timeList,
    AppComponents::Common::Types::Street::SegmentList const & segmentList,
    AppComponents::Common::Types::Routing::SamplingPointList const & samplingPointList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing sub routes";

    output_
        << "osm_id;route;length;cost;sourceNode;targetNode;sourceSamplingPointTime;targetSamplingPointTime;sourceSamplingPointCandidateIndex;targetSamplingPointCandidateIndex;sourceSamplingPointCandidateConsideredForwards;targetSamplingPointCandidateConsideredForwards\n";
    output_ << std::setprecision(14);
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
            output_ << segment.originId << ';';
            output_ << Core::Common::Geometry::toWkt(points) << ';';
            output_ << length << ';';
            output_ << cost << ';';
            output_ << nodeMap.at(source.node) << ';';
            output_ << nodeMap.at(target.node) << ';';
            output_ << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(source.samplingPoint.index).trackIndex)) << ';';
            output_ << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(target.samplingPoint.index).trackIndex)) << ';';
            output_ << source.samplingPoint.candidate.index << ';';
            output_ << target.samplingPoint.candidate.index << ';';
            output_ << source.samplingPoint.candidate.consideredForwards << ';';
            output_ << target.samplingPoint.candidate.consideredForwards << '\n';
        }
    }

    return true;
}

}  // namespace Applications::ExampleMatcher::Filter

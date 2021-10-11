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

namespace AppComponents::Common::Writer {

CsvRouteWriter::CsvRouteWriter(std::ostream & output) : Filter("CsvRouteWriter"), output_(output)
{
    setRequirements({"RouteList", "GraphEdgeMap", "NodeMap", "TimeList", "SegmentList", "SamplingPointList"});
    setOptionals({});
    setFulfillments({"route written"});
}

bool CsvRouteWriter::operator()(
    AppComponents::Common::Types::Routing::RouteList const & routeList,
    AppComponents::Common::Types::Graph::GraphEdgeMap const & graphEdgeMap,
    AppComponents::Common::Types::Graph::NodeMap const & nodeMap,
    AppComponents::Common::Types::Track::TimeList const & timeList,
    AppComponents::Common::Types::Street::SegmentList const & segmentList,
    AppComponents::Common::Types::Routing::SamplingPointList const & samplingPointList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing routes";

    output_
        << "osm_ids;route;length;cost;sourceNode;targetNode;sourceSamplingPointTime;targetSamplingPointTime;sourceSamplingPointCandidateIndex;targetSamplingPointCandidateIndex;sourceSamplingPointCandidateConsideredForwards;targetSamplingPointCandidateConsideredForwards;routeStartPoint;routeEndPoint\n";
    output_ << std::setprecision(14);
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
            output_ << str << ';';
        }
        output_ << Core::Common::Geometry::toWkt(points) << ';';
        output_ << totalLength << ';';
        output_ << totalCost << ';';
        output_ << nodeMap.at(source.node) << ';';
        output_ << nodeMap.at(target.node) << ';';
        output_ << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(source.samplingPoint.index).trackIndex)) << ';';
        output_ << Core::Common::Time::toIsoString(timeList.at(samplingPointList.at(target.samplingPoint.index).trackIndex)) << ';';
        output_ << source.samplingPoint.candidate.index << ';';
        output_ << target.samplingPoint.candidate.index << ';';
        output_ << source.samplingPoint.candidate.consideredForwards << ';';
        output_ << target.samplingPoint.candidate.consideredForwards << ';';
        output_ << Core::Common::Geometry::toWkt(points[0]) << ';';
        output_ << Core::Common::Geometry::toWkt(points[points.size() - 1]) << '\n';
    }

    return true;
}

}  // namespace AppComponents::Common::Writer
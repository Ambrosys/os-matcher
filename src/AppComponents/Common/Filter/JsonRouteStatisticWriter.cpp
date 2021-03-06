/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/JsonRouteStatisticWriter.h>

#include <Core/Common/Geometry/Conversion.h>
#include <Core/Common/Time/Helper.h>

#include <amblog/global.h>
#include <nlohmann/json.hpp>

#include <cassert>
#include <string>

namespace AppComponents::Common::Filter {

namespace {

    nlohmann::json toJson(
        Types::Routing::SamplingPointSelection const & samplingPointSelection, Types::Routing::SamplingPointList const & samplingPointList, Types::Track::TimeList const & timeList)
    {
        auto const & samplingPoint = samplingPointList.at(samplingPointSelection.index);
        std::string time;
        if (!timeList.empty())
            time = Core::Common::Time::toIsoString(timeList.at(samplingPoint.trackIndex));
        else
            time = std::to_string(samplingPoint.trackIndex);
        auto streetSegmentDistance = samplingPoint.candidates.at(samplingPointSelection.candidate.index).streetSegmentDistance;
        return nlohmann::json{
            {"index", samplingPointSelection.index},
            {"time", time},
            {"candidate",
             nlohmann::json{
                 {"index", samplingPointSelection.candidate.index},
                 {"considered-forwards", samplingPointSelection.candidate.consideredForwards},
                 {"distance", streetSegmentDistance}}},
            {"candidates-count", samplingPoint.candidates.size()}};
    }

}  // namespace

JsonRouteStatisticWriter::JsonRouteStatisticWriter(std::ostream & output) : Filter("JsonRouteStatisticWriter"), output_(output)
{
    setRequirements({"RoutingStatistic", "SamplingPointList"});
    setOptionals({"TimeList"});
    setFulfillments({"routing statistic written"});
}

bool JsonRouteStatisticWriter::operator()(
    Types::Routing::RoutingStatistic const & routingStatistic, Types::Routing::SamplingPointList const & samplingPointList, Types::Track::TimeList const & timeList)
{
    APP_LOG_TAG(noise, "I/O") << "Writing routing statistic";

    auto visited = nlohmann::json::array();
    for (auto const & [samplingPointsSelection, successfull] : routingStatistic.visited)
    {
        visited.push_back(nlohmann::json{
            {"successfull", successfull},
            {"source", toJson(samplingPointsSelection.source, samplingPointList, timeList)},
            {"target", toJson(samplingPointsSelection.target, samplingPointList, timeList)}});
    }

    auto calculated = nlohmann::json::array();
    for (auto const & [samplingPointsSelection, calculatedRouteStatistic] : routingStatistic.calculated)
    {
        calculated.push_back(nlohmann::json{
            {"source", toJson(samplingPointsSelection.source, samplingPointList, timeList)},
            {"target", toJson(samplingPointsSelection.target, samplingPointList, timeList)},
            {"cost", calculatedRouteStatistic.cost},
            {"length", calculatedRouteStatistic.length},
            {"sub-routes-count", calculatedRouteStatistic.subRoutesCount}});
    }

    nlohmann::json json{{"calculated", calculated}, {"visited", visited}};

    output_ << json.dump(2);

    return true;
}

}  // namespace AppComponents::Common::Filter

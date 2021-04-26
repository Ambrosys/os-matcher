#pragma once

#include <AppComponents/Common/Filter/Routing/Comparators.h>
#include <AppComponents/Common/Filter/Routing/Helper.h>
#include <AppComponents/Common/Filter/Routing/SamplingPointRouter.h>
#include <AppComponents/Common/Filter/Routing/Types.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>

#include <memory>
#include <unordered_map>

namespace AppComponents::Common::Filter::Routing {

class BacktrackRouter
{
public:
    struct Session;

    struct Configuration
    {
        double maxBacktrackingDistance;
    };

    BacktrackRouter(
        SamplingPointRouter const & router, Configuration const configuration, Types::Routing::SamplingPointList const & samplingPointList, Types::Track::TimeList const & timeList)
      : router_(router), configuration_(configuration), samplingPointList_(samplingPointList), timeList_(timeList)
    {
    }

    void operator()(
        size_t sourceSamplingPointIndexMinimum,
        size_t sourceSamplingPointIndexStart,
        size_t targetSamplingPointIndexGoal,
        std::unordered_set<size_t> const & skippedSamplingPoints,
        SamplingPointRouter::RouteMap & routeMap,
        Types::Routing::RouteList & routeList,
        Types::Routing::RoutingStatistic & routingStatistic) const;

private:
    SamplingPointRouter const & router_;
    Configuration const configuration_;
    Types::Routing::SamplingPointList const & samplingPointList_;
    Types::Track::TimeList const & timeList_;  // TODO: only for debugging

    /**
     * Routes in normal mode.
     * @return The route if the goal is not yet reached and still reachable.
     */
    std::tuple<RouteResult, size_t> routeProcess(size_t sourceSamplingPoint, Session & session) const;

    /**
     * Routes in skip mode.
     * @return The route if the goal is not yet reached and still reachable.
     */
    std::tuple<RouteResult, size_t> backtrackProcess(size_t sourceSamplingPoint, Session & session) const;
};

}  // namespace AppComponents::Common::Filter::Routing

/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Matcher/Routing/BacktrackRouter.h>
#include <AppComponents/Common/Matcher/Routing/Comparators.h>
#include <AppComponents/Common/Matcher/Routing/Generic/Skipper.h>
#include <AppComponents/Common/Matcher/Routing/Helper.h>
#include <AppComponents/Common/Matcher/Routing/Types.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>

namespace AppComponents::Common::Matcher::Routing {

class SkipRouter
{
public:
    struct Session;

    struct Configuration
    {
        double maxBacktrackingDistance;
        Generic::Skipper::Strategy skipStrategy;
    };

    SkipRouter(
        BacktrackRouter const & router, Configuration const configuration, Types::Routing::SamplingPointList const & samplingPointList, Types::Track::TimeList const & timeList)
      : router_(router), configuration_(configuration), samplingPointList_(samplingPointList), timeList_(timeList)
    {
    }

    void operator()(
        size_t sourceSamplingPointIndexStart,
        size_t targetSamplingPointIndexGoal,
        Types::Routing::RouteList & routeList,
        Types::Routing::RoutingStatistic & routingStatistic) const;

private:
    BacktrackRouter const & router_;
    Configuration const configuration_;
    Types::Routing::SamplingPointList const & samplingPointList_;
    Types::Track::TimeList const & timeList_;  // TODO: only for debugging

    /**
     * Routes in normal mode.
     * @return The \c RouteResult and reached sampling point index.
     */
    std::tuple<RouteResult, size_t> routeProcess(size_t sourceSamplingPoint, Session & session) const;

    /**
     * Routes in skip mode.
     * @return The \c RouteResult and reached sampling point index.
     */
    std::tuple<RouteResult, size_t> skipProcess(size_t sourceSamplingPoint, Session & session) const;
};

}  // namespace AppComponents::Common::Matcher::Routing

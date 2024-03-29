/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Track/Time.h>  // TODO: only for debugging

namespace AppComponents::Common::Matcher::Routing {

class SkipRouter;

class PiecewiseRouter
{
public:
    PiecewiseRouter(SkipRouter const & router, Types::Routing::SamplingPointList const &, Types::Track::TimeList const &);
    bool operator()(Types::Routing::RouteList &, Types::Routing::RoutingStatistic &);

private:
    SkipRouter const & router_;
    Types::Routing::SamplingPointList const & samplingPointList_;
    Types::Track::TimeList const & timeList_;  // TODO: only for debugging
};

}  // namespace AppComponents::Common::Matcher::Routing

#pragma once

#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Track/Time.h>  // TODO: only for debugging

namespace AppComponents::Common::Filter::Routing {

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

}  // namespace AppComponents::Common::Filter::Routing

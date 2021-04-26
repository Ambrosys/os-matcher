#pragma once

#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Track/Time.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Filter {

class JsonRouteStatisticWriter : public ambpipeline::Filter
{
public:
    JsonRouteStatisticWriter(std::ostream & output);
    bool operator()(Types::Routing::RoutingStatistic const &, Types::Routing::SamplingPointList const &, Types::Track::TimeList const &);

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Filter

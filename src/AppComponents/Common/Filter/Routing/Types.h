#pragma once

#include <AppComponents/Common/Filter/Routing/Generic/Skipper.h>
#include <AppComponents/Common/Types/Routing/Edge.h>

#include <Generic/Hash/MakeHashable.h>

namespace AppComponents::Common::Filter::Routing {

using SamplingPointSkipStrategy = Generic::Skipper::Strategy;

enum class RouteClusterPreference { cheapest, shortest };

struct SamplingPointsSelection
{
    Types::Routing::SamplingPointSelection source;
    Types::Routing::SamplingPointSelection target;

    inline bool operator==(SamplingPointsSelection const & other) const { return source == other.source && target == other.target; }
};

}  // namespace AppComponents::Common::Filter::Routing

MAKE_HASHABLE(
    AppComponents::Common::Filter::Routing::SamplingPointsSelection,
    t.source.index,
    t.source.candidate.index,
    t.source.candidate.consideredForwards,
    t.target.index,
    t.target.candidate.index,
    t.target.candidate.consideredForwards)

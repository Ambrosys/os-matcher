/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Matcher/Routing/Generic/Skipper.h>
#include <AppComponents/Common/Types/Routing/Edge.h>

#include <Generic/Hash/MakeHashable.h>

namespace AppComponents::Common::Matcher::Routing {

using SamplingPointSkipStrategy = Generic::Skipper::Strategy;

enum class RouteClusterPreference { cheapest, shortest };

struct SamplingPointsSelection
{
    Types::Routing::SamplingPointSelection source;
    Types::Routing::SamplingPointSelection target;

    inline bool operator==(SamplingPointsSelection const & other) const { return source == other.source && target == other.target; }
};

}  // namespace AppComponents::Common::Matcher::Routing

MAKE_HASHABLE(
    AppComponents::Common::Matcher::Routing::SamplingPointsSelection,
    t.source.index,
    t.source.candidate.index,
    t.source.candidate.consideredForwards,
    t.target.index,
    t.target.candidate.index,
    t.target.candidate.consideredForwards)

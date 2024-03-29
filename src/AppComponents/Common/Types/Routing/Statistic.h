
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Matcher/Routing/Types.h>

#include <unordered_map>
#include <unordered_set>

namespace AppComponents::Common::Types::Routing {

struct CalculatedRouteStatistic
{
    // TODO: consider keeping whole Route
    double cost;
    double length;
    size_t subRoutesCount;
};

/**
 * Holds additional routing data per sampling point route data.
 */
struct RoutingStatistic
{
    std::unordered_map<Matcher::Routing::SamplingPointsSelection, CalculatedRouteStatistic> calculated;
    std::vector<std::pair<Matcher::Routing::SamplingPointsSelection, bool>> visited;
};

}  // namespace AppComponents::Common::Types::Routing

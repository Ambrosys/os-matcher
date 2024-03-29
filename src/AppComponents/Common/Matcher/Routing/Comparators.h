/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Matcher/Routing/Types.h>
#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Routing/Edge.h>

#include <memory>

namespace AppComponents::Common::Matcher::Routing {

struct BestRouteComparator
{
    BestRouteComparator(RouteClusterPreference routeClusterPreference) : routeClusterPreference_(routeClusterPreference) {}
    bool operator()(std::shared_ptr<Types::Routing::Route> const & a, std::shared_ptr<Types::Routing::Route> const & b) const;

private:
    RouteClusterPreference routeClusterPreference_;
};

struct BestSimilarRouteComparator
{
    bool operator()(std::shared_ptr<Types::Routing::Route> const & a, std::shared_ptr<Types::Routing::Route> const & b) const;
};

bool isSimilar(Types::Routing::Route const & a, Types::Routing::Route const & b, double const maxLengthDifference, Types::Graph::GraphEdgeMap const & graphEdgeMap);

}  // namespace AppComponents::Common::Matcher::Routing

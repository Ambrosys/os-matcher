/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/Routing/Comparators.h>
#include <AppComponents/Common/Filter/Routing/Helper.h>

#include <cassert>

namespace AppComponents::Common::Filter::Routing {

namespace {

    bool isContained(size_t const streetIndex, Types::Routing::Route const & route, Types::Graph::GraphEdgeMap const & graphEdgeMap)
    {
        for (auto const & subRoute : route.subRoutes)
            if (graphEdgeMap.at(subRoute.edge).streetIndex == streetIndex)
                return true;
        return false;
    }

}  // namespace

bool isSimilar(Types::Routing::Route const & a, Types::Routing::Route const & b, double const maxLengthDifference, Types::Graph::GraphEdgeMap const & graphEdgeMap)
{
    assert(!a.subRoutes.empty());
    assert(!b.subRoutes.empty());
    auto const & aHead = graphEdgeMap.at(a.subRoutes[0].edge).streetIndex;
    auto const & aTail = graphEdgeMap.at(a.subRoutes[a.subRoutes.size() - 1].edge).streetIndex;
    auto const & bHead = graphEdgeMap.at(b.subRoutes[0].edge).streetIndex;
    auto const & bTail = graphEdgeMap.at(b.subRoutes[b.subRoutes.size() - 1].edge).streetIndex;
    bool found;

    // check that the maximum distance difference is not surpassed
    if (std::abs(a.length() - b.length()) > maxLengthDifference)
        return false;

    // check that one head is contained in the other route
    found = isContained(bHead, a, graphEdgeMap);
    if (!found)
        found = isContained(aHead, b, graphEdgeMap);
    if (!found)
        return false;

    // check that one tail is contained in the other route
    found = isContained(bTail, a, graphEdgeMap);
    if (!found)
        found = isContained(aTail, b, graphEdgeMap);
    if (!found)
        return false;

    // check that one head/tail is not contained in itself
    if (isSelfIntersectingRoute(a))
        return false;
    if (isSelfIntersectingRoute(b))
        return false;

    return true;
};

bool BestRouteComparator::operator()(std::shared_ptr<Types::Routing::Route> const & a, std::shared_ptr<Types::Routing::Route> const & b) const
{
    assert(!a->subRoutes.empty());
    assert(!b->subRoutes.empty());
    switch (routeClusterPreference_)
    {
        case RouteClusterPreference::cheapest:
            if (a->cost() != b->cost())
                return a->cost() < b->cost();
            else if (a->length() != b->length())
                return a->length() < b->length();
            else
                return a->numPoints() < b->numPoints();
        case RouteClusterPreference::shortest:
            if (a->length() != b->length())
                return a->length() < b->length();
            else if (a->cost() != b->cost())
                return a->cost() < b->cost();
            else
                return a->numPoints() < b->numPoints();
    }
    assert(false);
    return false;
};

bool BestSimilarRouteComparator::operator()(std::shared_ptr<Types::Routing::Route> const & a, std::shared_ptr<Types::Routing::Route> const & b) const
{
    assert(!a->subRoutes.empty());
    assert(!b->subRoutes.empty());
    // TODO: somehow consider costs of candidate, not only its rank.
    auto aValue = a->source.samplingPoint.candidate.index + a->target.samplingPoint.candidate.index;
    auto bValue = b->source.samplingPoint.candidate.index + b->target.samplingPoint.candidate.index;
    if (aValue == bValue)
        return a->source.samplingPoint.candidate.index < b->source.samplingPoint.candidate.index;
    return aValue < bValue;
}

}  // namespace AppComponents::Common::Filter::Routing

#pragma once

#include <AppComponents/Common/Filter/Routing/Types.h>
#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Routing/Edge.h>

#include <memory>

namespace AppComponents::Common::Filter::Routing {

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

}  // namespace AppComponents::Common::Filter::Routing


#pragma once

#include <Core/Common/Geometry/Types.h>
#include <Core/Graph/Graph.h>

#include <cstddef>
#include <memory>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>

namespace AppComponents::Common::Types::Routing {

using Edge = Core::Graph::Edge;
using Node = Core::Graph::Node;

struct SubRoute
{
    Edge edge;
    double cost;
    Core::Common::Geometry::LineString route;
    double length;
};

struct SamplingPointCandidateSelection
{
    size_t index;
    bool consideredForwards;
    inline bool operator==(SamplingPointCandidateSelection const & other) const { return index == other.index && consideredForwards == other.consideredForwards; }
};

struct SamplingPointSelection
{
    size_t index;
    SamplingPointCandidateSelection candidate;
    inline bool operator==(SamplingPointSelection const & other) const { return index == other.index && candidate == other.candidate; }
};

struct RouteNode
{
    Node node;
    SamplingPointSelection samplingPoint;
};

struct Route
{
    RouteNode source;
    RouteNode target;
    std::vector<SubRoute> subRoutes;
    Route(RouteNode const & source, RouteNode const & target, std::vector<SubRoute> const & subRoutes) : source(source), target(target), subRoutes(subRoutes) {}
    double cost() const
    {
        if (!cost_)
            cost_ = std::accumulate(subRoutes.begin(), subRoutes.end(), 0.0, [](double sum, auto const & subRoute) { return sum + subRoute.cost; });
        return *cost_;
    }
    double length() const
    {
        if (!length_)
            length_ = std::accumulate(subRoutes.begin(), subRoutes.end(), 0.0, [](double sum, auto const & subRoute) { return sum + subRoute.length; });
        return *length_;
    }
    size_t numPoints() const
    {
        if (!numPoints_)
            numPoints_ = std::accumulate(subRoutes.begin(), subRoutes.end(), 0, [](size_t sum, auto const & subRoute) { return sum + subRoute.route.size(); });
        return *numPoints_;
    }

private:
    mutable std::optional<double> cost_;
    mutable std::optional<double> length_;
    mutable std::optional<size_t> numPoints_;
};

using RouteList = std::vector<std::shared_ptr<Route>>;

}  // namespace AppComponents::Common::Types::Routing

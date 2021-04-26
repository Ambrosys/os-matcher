#pragma once

#include <Core/Graph/Graph.h>
#include <Core/Graph/Routing/PathView.h>

#include <functional>

namespace Core::Graph::Routing {

using CostFunction = std::function<double(Core::Graph::Edge)>;

using FilterFunction = std::function<bool(PathView const &)>;

class RoutingAlgorithm
{
public:
    RoutingAlgorithm() : costFunction_{[](Core::Graph::Edge) { return 1.0f; }}, filterFunction_{[](PathView const &) { return true; }} {}

    virtual ~RoutingAlgorithm() {}

    virtual PathView operator()(Core::Graph::Node source, Core::Graph::Node destination) = 0;

    virtual PathView run(Core::Graph::Node source, Core::Graph::Node destination) { return this->operator()(source, destination); }

    virtual RoutingAlgorithm & setCost(CostFunction costFuncton)
    {
        costFunction_ = costFuncton;
        return *this;
    }

    virtual RoutingAlgorithm & setFilter(FilterFunction filterFunction)
    {
        filterFunction_ = filterFunction;
        return *this;
    }

protected:
    CostFunction costFunction_;
    FilterFunction filterFunction_;
};

}  // namespace Core::Graph::Routing
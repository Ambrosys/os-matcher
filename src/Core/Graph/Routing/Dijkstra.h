/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Graph/Routing/Algorithm.h>
#include <Core/Graph/Routing/PathView.h>

#include <functional>
#include <memory>
#include <queue>
#include <unordered_set>

namespace Core::Graph::Routing {

class Dijkstra : public RoutingAlgorithm
{
public:
    class PathNode : public PathViewImpl
    {
    public:
        PathNode(Core::Graph::Edge edge, double cost, std::shared_ptr<PathNode> const & previous);
        Core::Graph::Edge edge() const override;
        double cost() const override;
        PathViewImpl * previous() const override;

        Core::Graph::Edge edge_;
        double cost_;
        std::shared_ptr<PathNode> previous_;
    };

    Dijkstra(Core::Graph::Graph const & graph);

    PathView operator()(Core::Graph::Node source, Core::Graph::Node destination) override;

private:
    using Container = std::deque<std::shared_ptr<PathNode>>;
    using GreaterFunction = std::greater<std::shared_ptr<Dijkstra::PathNode>>;
    using Frontier = std::priority_queue<std::shared_ptr<PathNode>, Container, GreaterFunction>;
    using Visited = std::unordered_set<Core::Graph::Edge>;

    void init(Core::Graph::Node source);
    // std::shared_ptr<PathNode> rollback( std::shared_ptr<PathNode> path );
    std::vector<std::shared_ptr<PathNode>> explore(std::shared_ptr<PathNode> const & parent);

    bool reached(std::shared_ptr<PathNode> const & pathNode, Core::Graph::Node graphNode);

    Core::Graph::Graph const & graph_;

    Frontier frontier_;
    Visited visited_;
};

bool operator>(std::shared_ptr<Dijkstra::PathNode> const & lhs, std::shared_ptr<Dijkstra::PathNode> const & rhs);

}  // namespace Core::Graph::Routing
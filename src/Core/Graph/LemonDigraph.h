/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Graph/Graph.h>

#include <lemon/list_graph.h>

namespace Core::Graph {

namespace Routing {
    class Dijkstra;
}

class LemonDigraph : public Graph
{
public:
    Node createNode() override;
    void remove(Node node) override;

    Edge addEdge(Node source, Node target) override;
    void remove(Edge edge) override;

    bool has(Node node) const override;
    bool has(Edge edge) const override;

    Node source(Edge edge) const override;
    Node target(Edge edge) const override;

    std::vector<Edge> outEdges(Node node) const override;
    std::vector<Edge> inEdges(Node node) const override;

    Node fromLemonNode(lemon::ListDigraph::Node lemonNode) const;
    lemon::ListDigraph::Node toLemonNode(Node node) const;
    Edge fromLemonArc(lemon::ListDigraph::Arc lemonArc) const;
    lemon::ListDigraph::Arc toLemonArc(Edge edge) const;

private:
    lemon::ListDigraph graph_;
};

}  // namespace Core::Graph
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Graph/LemonDigraph.h>

#include <lemon/dijkstra.h>

#include <cassert>
#include <utility>

namespace Core::Graph {

Node LemonDigraph::createNode()
{
    return fromLemonNode(graph_.addNode());
}

void LemonDigraph::remove(Node node)
{
    graph_.erase(toLemonNode(node));
}

Edge LemonDigraph::addEdge(Node source, Node target)
{
    auto lemonArc = graph_.addArc(toLemonNode(source), toLemonNode(target));
    return fromLemonArc(lemonArc);
}

void LemonDigraph::remove(Edge edge)
{
    graph_.erase(toLemonArc(edge));
}

bool LemonDigraph::has(Node node) const
{
    return graph_.valid(toLemonNode(node));
}

bool LemonDigraph::has(Edge edge) const
{
    return graph_.valid(toLemonArc(edge));
}

Node LemonDigraph::source(Edge edge) const
{
    return fromLemonNode(graph_.source(toLemonArc(edge)));
}

Node LemonDigraph::target(Edge edge) const
{
    return fromLemonNode(graph_.target(toLemonArc(edge)));
}

std::vector<Edge> LemonDigraph::outEdges(Node node) const
{
    auto outEdges = std::vector<Edge>{};
    for (lemon::ListDigraph::OutArcIt lemonArc(graph_, toLemonNode(node)); lemonArc != lemon::INVALID; ++lemonArc)
    {
        outEdges.emplace_back(fromLemonArc(lemonArc));
    }
    return outEdges;
}

std::vector<Edge> LemonDigraph::inEdges(Node node) const
{
    auto inEdges = std::vector<Edge>{};
    for (lemon::ListDigraph::InArcIt lemonArc(graph_, toLemonNode(node)); lemonArc != lemon::INVALID; ++lemonArc)
    {
        inEdges.emplace_back(fromLemonArc(lemonArc));
    }
    return inEdges;
}

Node LemonDigraph::fromLemonNode(lemon::ListDigraph::Node lemonNode) const
{
    return this->newNode(static_cast<std::size_t>(lemon::ListDigraph::id(lemonNode)));
}

lemon::ListDigraph::Node LemonDigraph::toLemonNode(Node node) const
{
    return lemon::ListDigraph::nodeFromId(static_cast<int>(node.id()));
}

Edge LemonDigraph::fromLemonArc(lemon::ListDigraph::Arc lemonArc) const
{
    return this->newEdge(static_cast<std::size_t>(lemon::ListDigraph::id(lemonArc)));
}

lemon::ListDigraph::Arc LemonDigraph::toLemonArc(Edge edge) const
{
    return lemon::ListDigraph::arcFromId(static_cast<int>(edge.id()));
}

}  // namespace Core::Graph

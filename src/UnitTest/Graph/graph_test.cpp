/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Graph/Graph.h>
#include <Core/Graph/LemonDigraph.h>

#include <catch2/catch.hpp>

#include <memory>

using namespace Core::Graph;

enum class GraphConcept
{
    Graph,
    Digraph,
    // MultiDigraph
};

struct TestCandidate
{
    std::string description;
    GraphConcept concept;
    std::shared_ptr<Graph> graph;
};

std::vector<TestCandidate> graph_implementations = {TestCandidate{"LemonDigraph", GraphConcept::Digraph, std::make_shared<LemonDigraph>()}};

void general_graph_tests(Graph & graph)
{
    WHEN("node is added")
    {
        auto node = graph.createNode();
        THEN("node has to be in graph") { CHECK(graph.has(node)); }
        WHEN("node is removed again")
        {
            graph.remove(node);
            THEN("node must not be in graph") { CHECK_FALSE(graph.has(node)); }
        }
        WHEN("an edge on that node is added")
        {
            auto edge = graph.addEdge(node, node);
            THEN("soure and target of that edge have to be that node")
            {
                CHECK(graph.source(edge) == node);
                CHECK(graph.target(edge) == node);
            }
            THEN("there has to be exactly one out- and one in-edge equal to edge")
            {
                auto outEdges = graph.outEdges(node);
                auto inEdges = graph.inEdges(node);
                CHECK(outEdges.size() == 1);
                CHECK(inEdges.size() == 1);
                CHECK(outEdges[0] == inEdges[0]);
            }
        }
    }

    WHEN("two nodes and an edge between them is added")
    {
        auto source = graph.createNode();
        auto target = graph.createNode();
        auto edge = graph.addEdge(source, target);
        THEN("edge has to be in graph") { CHECK(graph.has(edge)); }
        THEN("edge must have a source node") { CHECK(source == graph.source(edge)); }
        THEN("edge must have a target node") { CHECK(target == graph.target(edge)); }
        THEN("edge must be the only out-edge of source node")
        {
            auto outEdges = graph.outEdges(source);
            CHECK(outEdges.size() == 1);
            CHECK(edge == outEdges[0]);
        }
        THEN("edge must be the only in-edge of target node")
        {
            auto inEdges = graph.inEdges(target);
            CHECK(inEdges.size() == 1);
            CHECK(edge == inEdges[0]);
        }
        WHEN("edge is removed again")
        {
            graph.remove(edge);
            THEN("edge must not be in graph") { CHECK_FALSE(graph.has(edge)); }
        }
    }
}

void digraph_tests(Graph & graph)
{
    WHEN("two nodes and two edges are added between them")
    {
        auto nodes = std::vector<Node>{graph.createNode(), graph.createNode()};
        auto edges = std::vector<Edge>{graph.addEdge(nodes[0], nodes[1]), graph.addEdge(nodes[1], nodes[0])};
        THEN("the edges have to be opposit pairs")
        {
            CHECK(graph.source(edges[0]) == graph.target(edges[1]));
            CHECK(graph.target(edges[0]) == graph.source(edges[1]));
        }
    }
}

// TODO(jg): Test EdgeMaps.

SCENARIO("Test Graph implementations", "[Graph]")
{
    for (auto & impl : graph_implementations)
    {
        GIVEN(impl.description) { general_graph_tests(*impl.graph); }
    }

    for (auto & impl : graph_implementations)
    {
        if (impl.concept != GraphConcept::Digraph)
            continue;

        GIVEN(impl.description) { digraph_tests(*impl.graph); }
    }
}

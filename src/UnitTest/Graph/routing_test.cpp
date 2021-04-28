#include <Core/Graph/LemonDigraph.h>
#include <Core/Graph/Routing/Algorithm.h>
#include <Core/Graph/Routing/Dijkstra.h>

#include <catch2/catch.hpp>

#include <memory>
#include <unordered_map>

using namespace Core::Graph;
using namespace Core::Graph::Routing;

namespace {

using FactoryFunction = std::function<std::unique_ptr<RoutingAlgorithm>(Graph const &)>;

template <typename Algorithm>
std::unique_ptr<RoutingAlgorithm> algorithmFactory(Graph const & graph)
{
    return std::make_unique<Algorithm>(graph);
}

struct TestCandidate
{
    std::string description;
    FactoryFunction factory;
};

std::vector<TestCandidate> routing_implementations = {TestCandidate{"Dijkstra", algorithmFactory<Dijkstra>}};

void require_path_equal(PathView pathView, std::vector<Edge> const & edgeContainer)
{
    REQUIRE(pathView.size() == edgeContainer.size());
    auto pvIt = pathView.begin();
    auto ecIt = std::rbegin(edgeContainer);
    for (; ecIt != std::rend(edgeContainer); ++ecIt, ++pvIt)
    {
        REQUIRE(*ecIt == pvIt->edge());
    }
}

bool tail_equal(PathView pathView, std::vector<Edge> const & edgeContainer)
{
    if (pathView.size() < edgeContainer.size())
        return false;
    auto pvIt = pathView.begin();
    auto ecIt = std::rbegin(edgeContainer);
    for (; ecIt != std::rend(edgeContainer); ++ecIt, ++pvIt)
    {
        if (not(*ecIt == pvIt->edge()))
            return false;
    }
    return true;
}

}  // namespace

void route_on_connected_dag(FactoryFunction factory)
{
    WHEN("on a two-node dag")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto source = graph.createNode();
        auto target = graph.createNode();
        auto edge = graph.addEdge(source, target);

        THEN("routing between the two nodes is possible")
        {
            auto path = algorithm->run(source, target);
            CHECK(not path.empty());
            require_path_equal(path, {edge});
        }
    }
    WHEN("on a connected dag")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 3; ++i)
            nodes.push_back(graph.createNode());
        auto sink = graph.createNode();
        graph.addEdge(nodes[0], nodes[1]);
        graph.addEdge(nodes[0], nodes[2]);
        graph.addEdge(nodes[1], nodes[2]);
        graph.addEdge(nodes[2], sink);

        THEN("routing to the sink from every other node is possible")
        {
            for (auto node : nodes)
            {
                auto path = algorithm->run(node, sink);
                CHECK(not path.empty());
                REQUIRE(graph.source(path.back().edge()) == node);
                REQUIRE(graph.target(path.front().edge()) == sink);
            }
        }
        THEN("Routing from the sink to every other node is impossible")
        {
            for (auto node : nodes)
            {
                auto path = algorithm->run(sink, node);
                REQUIRE(path.empty());
            }
        }
    }
}

void route_to_unreachable_test(FactoryFunction factory)
{
    WHEN("on a single node")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto node = graph.createNode();

        THEN("routing on that node is impossible")
        {
            auto path = algorithm->run(node, node);
            REQUIRE(path.empty());
        }
    }
    WHEN("on two disconnected nodes")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto source = graph.createNode();
        auto target = graph.createNode();

        THEN("routing between them is impossible")
        {
            auto path = algorithm->run(source, target);
            REQUIRE(path.empty());
        }
    }
    WHEN("on a three-node dag with a disconnected destination node")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 4; ++i)
            nodes.push_back(graph.createNode());
        graph.addEdge(nodes[0], nodes[1]);
        graph.addEdge(nodes[1], nodes[2]);
        graph.addEdge(nodes[0], nodes[2]);
        auto const & disconnected = nodes[3];

        THEN("routing from any node to the destination node is impossible")
        {
            for (auto source : nodes)
            {
                auto path = algorithm->run(source, disconnected);
                REQUIRE(path.empty());
            }
        }
    }
}

void route_on_cyclic_digraph(FactoryFunction factory)
{
    WHEN("on a two-node cycle")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto source = graph.createNode();
        auto other = graph.createNode();
        auto toTarget = graph.addEdge(source, other);
        auto toSource = graph.addEdge(other, source);

        WHEN("routing from the source to itself")
        {
            auto path = algorithm->run(source, source);

            THEN("a path exists and it has two edges")
            {
                CHECK(not path.empty());
                require_path_equal(path, {toTarget, toSource});
            }
            THEN("the first and last node in the path are the source")
            {
                REQUIRE(graph.source(path.back().edge()) == source);
                REQUIRE(graph.target(path.front().edge()) == source);
            }
            THEN("the central node in the path is not the source")
            {
                REQUIRE(graph.target(path.back().edge()) == other);
                REQUIRE(graph.source(path.front().edge()) == other);
            }
        }
    }
    WHEN("on a three-node cycle with a disconnected destination node")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 4; ++i)
            nodes.push_back(graph.createNode());
        graph.addEdge(nodes[0], nodes[1]);
        graph.addEdge(nodes[1], nodes[2]);
        graph.addEdge(nodes[2], nodes[0]);
        auto const & disconnected = nodes[3];

        THEN("routing from any node to the destination node is impossible")
        {
            for (auto source : nodes)
            {
                auto path = algorithm->run(source, disconnected);
                REQUIRE(path.empty());
            }
        }
    }
}

void route_with_costfunction(FactoryFunction factory)
{
    WHEN("routing on a triangle shaped dag")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 3; ++i)
            nodes.push_back(graph.createNode());
        auto edges = std::vector<Edge>{};
        edges.push_back(graph.addEdge(nodes[0], nodes[1]));
        edges.push_back(graph.addEdge(nodes[0], nodes[2]));
        edges.push_back(graph.addEdge(nodes[1], nodes[2]));

        WHEN("using uniform weight")
        {
            auto costFunction = [](Edge) { return 1.0f; };
            algorithm->setCost(costFunction);
            auto path = algorithm->run(nodes[0], nodes[1]);

            THEN("the shortest path is the one-edge path")
            {
                CHECK(not path.empty());
                require_path_equal(path, {edges[0]});
            }
        }
        WHEN("weighting the two-edge path less")
        {
            auto weight = std::unordered_map<Edge, double>{{edges[0], 0.5f}, {edges[1], 3.0f}, {edges[2], 1.0f}};
            auto costFunction = [&weight](Edge edge) { return weight[edge]; };
            algorithm->setCost(costFunction);
            auto path = algorithm->run(nodes[0], nodes[2]);

            THEN("the shortest path is the two-edge path")
            {
                CHECK(not path.empty());
                require_path_equal(path, {edges[0], edges[2]});
            }
        }
    }
    WHEN("routing on a triangle shaped dag with appendix sink")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 4; ++i)
            nodes.push_back(graph.createNode());
        auto edges = std::vector<Edge>{};
        edges.push_back(graph.addEdge(nodes[0], nodes[1]));
        edges.push_back(graph.addEdge(nodes[0], nodes[2]));
        edges.push_back(graph.addEdge(nodes[1], nodes[2]));
        edges.push_back(graph.addEdge(nodes[2], nodes[3]));

        WHEN("using uniform weight")
        {
            auto costFunction = [](Edge) { return 1.0f; };
            algorithm->setCost(costFunction);
            auto path = algorithm->run(nodes[0], nodes[3]);

            THEN("the shortest path is the two-edge path")
            {
                CHECK(not path.empty());
                require_path_equal(path, {edges[1], edges[3]});
            }
        }
        WHEN("weighting the three-edge path less")
        {
            auto weight = std::unordered_map<Edge, double>{
                {edges[0], 1.0f},
                {edges[1], 3.0f},
                {edges[2], 1.0f},
                {edges[3], 2.0f},
            };
            auto costFunction = [&weight](Edge edge) { return weight[edge]; };
            algorithm->setCost(costFunction);
            auto path = algorithm->run(nodes[0], nodes[3]);

            THEN("the shortest path is the three-edge path")
            {
                CHECK(not path.empty());
                require_path_equal(path, {edges[0], edges[2], edges[3]});
            }
        }
    }
}

void route_with_filterfunction(FactoryFunction factory)
{
    WHEN("routing on a triangle shaped dag with appendix sink")
    {
        auto graph = LemonDigraph();
        auto algorithm = factory(graph);
        auto nodes = std::vector<Node>{};
        for (unsigned int i = 0; i < 4; ++i)
            nodes.push_back(graph.createNode());
        auto edges = std::vector<Edge>{};
        edges.push_back(graph.addEdge(nodes[0], nodes[1]));
        edges.push_back(graph.addEdge(nodes[0], nodes[2]));
        edges.push_back(graph.addEdge(nodes[1], nodes[2]));
        edges.push_back(graph.addEdge(nodes[2], nodes[3]));
        std::cerr << "Out edges size is : " << graph.outEdges(graph.source(edges[0])).size() << '\n';
        WHEN("weighting the three-edge path less")
        {
            auto weight = std::unordered_map<Edge, double>{
                {edges[0], 1.0f},
                {edges[1], 3.0f},
                {edges[2], 1.0f},
                {edges[3], 2.0f},
            };
            auto costFunction = [&weight](Edge edge) { return weight[edge]; };
            algorithm->setCost(costFunction);

            WHEN("rejecting the last two edges of the three-edge path")
            {
                auto restriction = std::vector<Edge>{edges[2], edges[3]};
                auto filterFunction = [&restriction](PathView path)
                {
                    auto res = not tail_equal(path, restriction);
                    std::cerr << "filterFunction(): " << res << " : " << path << '\n';
                    return res;
                };
                algorithm->setFilter(filterFunction);

                auto path = algorithm->run(nodes[0], nodes[3]);

                THEN("the result path is the two-edge path")
                {
                    CHECK(not path.empty());
                    require_path_equal(path, {edges[1], edges[3]});
                }
            }
        }
    }
}

SCENARIO("Test routing algorithm implementations", "[Graph][Routing]")
{
    for (auto & impl : routing_implementations)
    {
        GIVEN(impl.description)
        {
            // route_on_connected_dag( impl.factory );
            // route_to_unreachable_test( impl.factory );
            // route_on_cyclic_digraph( impl.factory );
            // route_with_costfunction( impl.factory );
            route_with_filterfunction(impl.factory);
        }
    }
}

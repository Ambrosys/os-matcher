/*
 * Author: Fabian Sandoval Saldias (fabianvss@gmail.com)
 * Date: 2018-06-19
 */

#include <ambgraph/DependencyGraph.h>

#include <UnitTest/Helpers.h>
#include <catch2/catch.hpp>

using namespace ambgraph;

namespace {

using PointerIdMap = std::unordered_map<void const *, size_t>;
using NamePointerIdMap = std::unordered_map<std::string, PointerIdMap>;
using PointerSet = std::unordered_set<void const *>;

template <typename T>
void printNode(
    std::ostringstream & oss, NamePointerIdMap & pointersForNameMap, PointerSet & nodesSeenGlobally, PointerSet nodesSeen, typename DependencyGraph<T>::Node const & node)
{
    auto & pointersForName = pointersForNameMap.try_emplace(node.data.name, PointerIdMap{}).first->second;
    auto pointerIdForName = pointersForName.try_emplace(&node, pointersForName.size()).first->second;
    oss << node.data.name;
    if (pointerIdForName > 0)
        oss << '_' << pointerIdForName + 1;
    if (nodesSeen.find(&node) != nodesSeen.end())
    {
        oss << " [circular reference]";
        return;
    }
    if (nodesSeenGlobally.find(&node) != nodesSeenGlobally.end())
    {
        if (not node.children.empty())
            oss << " --> [...]";
        return;
    }
    nodesSeen.insert(&node);
    nodesSeenGlobally.insert(&node);
    if (not node.children.empty())
    {
        typename DependencyGraph<T>::NodeVector sortedChildren = node.children;
        std::sort(
            sortedChildren.begin(),
            sortedChildren.end(),
            [](std::shared_ptr<typename DependencyGraph<T>::Node> a, std::shared_ptr<typename DependencyGraph<T>::Node> b) { return a->data.name < b->data.name; });

        oss << " --> ";
        if (sortedChildren.size() > 1)
            oss << "{ ";
        for (size_t i = 0; i < sortedChildren.size(); ++i)
        {
            printNode<T>(oss, pointersForNameMap, nodesSeenGlobally, nodesSeen, *sortedChildren[i]);
            if (i < sortedChildren.size() - 1)
                oss << ", ";
        }
        if (sortedChildren.size() > 1)
            oss << " }";
    }
}

/**
 * Generates a testable one-line string representation from the graph.
 *
 * Adds indices to different nodes with the same name.
 * Sorts siblings alphabetically.
 * Detects and renderes circular references.
 *
 * Example outputs:
 * Track --> B --> { Heading --> C --> Track_3 --> A, Track_2 --> C --> [...] }
 * Track --> C --> Track_2 --> B --> { Heading --> C [circular reference], Track_3 --> A }
 */
template <typename T>
std::string toString(typename DependencyGraph<T>::Root const & root)
{
    std::ostringstream oss;
    NamePointerIdMap pointersForNameMap;
    PointerSet nodesSeenGlobally;
    typename DependencyGraph<T>::NodeVector sortedChildren = root.children;
    std::sort(
        sortedChildren.begin(),
        sortedChildren.end(),
        [](std::shared_ptr<typename DependencyGraph<T>::Node> const & a, std::shared_ptr<typename DependencyGraph<T>::Node> const & b) { return a->data.name < b->data.name; });
    for (size_t i = 0; i < sortedChildren.size(); ++i)
    {
        printNode<T>(oss, pointersForNameMap, nodesSeenGlobally, {}, *sortedChildren[i]);
        if (i < sortedChildren.size() - 1)
            oss << ", ";
    }
    return oss.str();
}

std::string toString(std::vector<std::string> const & names)
{
    std::vector<std::string> sorted{names.begin(), names.end()};
    std::sort(sorted.begin(), sorted.end());

    std::ostringstream oss;
    for (size_t i = 0; i < sorted.size(); ++i)
    {
        if (i)
            oss << ", ";
        oss << sorted[i];
    }
    return oss.str();
}

template <typename T>
std::string toString(typename DependencyGraph<T>::UnfulfilledItems const & unfulfilledItems)
{
    typename DependencyGraph<T>::UnfulfilledItems sorted{unfulfilledItems};
    //std::transform( nodeDataVector.begin(), nodeDataVector.end(), std::back_inserter( sorted.end() ), []( auto const & pair ){ return std::pair{ pair.first, pair.second }; } );
    std::sort(sorted.begin(), sorted.end(), [](auto const & a, auto const & b) { return a.first.name < b.first.name; });

    std::ostringstream oss;
    for (size_t i = 0; i < sorted.size(); ++i)
    {
        if (i)
            oss << ", ";
        oss << sorted[i].first.name;
        if (!sorted[i].second.empty())
            oss << '(' << toString(sorted[i].second) << ')';
    }
    return oss.str();
}

template <typename T>
std::string toString(typename DependencyGraph<T>::NodeDataVector const & nodeDataVector)
{
    typename DependencyGraph<T>::NodeDataVector sorted{nodeDataVector.begin(), nodeDataVector.end()};
    std::sort(sorted.begin(), sorted.end(), [](typename DependencyGraph<T>::NodeData const & a, typename DependencyGraph<T>::NodeData const & b) { return a.name < b.name; });

    std::ostringstream oss;
    for (size_t i = 0; i < sorted.size(); ++i)
    {
        if (i)
            oss << ", ";
        oss << sorted[i].name;
    }
    return oss.str();
}

}  // namespace

// clang-format off

TEST_CASE( "DependencyGraph builds correctly", "[DependencyGraph]" )
{
    using DependencyGraph = DependencyGraph<int>;
    std::vector<DependencyGraph::Item> items;
    auto featureDataFactory = []( std::string ){ return DependencyGraph::Data{ 0 }; };

    SECTION( "single node present" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node present (with unfulfilled optional)" )
    {
        items.push_back( { { 0, "A" }, {}, {"x"}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node present (with fulfillment)" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {"x"} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "x --> A" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node present (with identical optional and fulfillment)" )
    {
        items.push_back( { { 0, "A" }, {}, {"x"}, {"x"} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" ); // no root node should be found
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node removed (with unfulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {"x"}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" );
        REQUIRE( toString<int>( unfulfilled ) == "A(x)" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node removed (with two unfulfilled requirements)" )
    {
        items.push_back( { { 0, "A" }, {"x","y"}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" );
        REQUIRE( toString<int>( unfulfilled ) == "A(x, y)" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "single node removed (with identical requirement and optional)" )
    {
        items.push_back( { { 0, "A" }, {"x"}, {"x"}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" );
        REQUIRE( toString<int>( unfulfilled ) == "A(x)" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "nodes with same fulfillment" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {"x"} } );
        items.push_back( { { 0, "B" }, {}, {}, {"x"} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "x --> { A, B }" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "nodes with same requirement (feature)" )
    {
        items.push_back( { { 0, "A" }, {"x"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"x"}, {}, {} } );
        items.push_back( { { 0, "C" }, {},    {}, {"x"} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> x --> C, B --> x --> [...]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "nodes with same requirement (item)" )
    {
        items.push_back( { { 0, "A" }, {"C"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"C"}, {}, {} } );
        items.push_back( { { 0, "C" }, {},    {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> C, B --> C" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes parallel" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {} } );
        items.push_back( { { 0, "B" }, {}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A, B" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes parallel (with optional unfulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {}, {"x"}, {} } );
        items.push_back( { { 0, "B" }, {}, {},    {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A, B" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes sequential (with fulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {"B"}, {}, {} } );
        items.push_back( { { 0, "B" }, {}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> B" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes sequential (with optional fulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {}, {"B"}, {} } );
        items.push_back( { { 0, "B" }, {}, {},    {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> B" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes sequential (with fulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {"B"} } );
        items.push_back( { { 0, "B" }, {}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "B --> A" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "two nodes removed (with unfulfilled requirements and one fulfilled requirement)" )
    {
        items.push_back( { { 0, "A" }, {"x","B"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"x"},     {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" );
        REQUIRE( toString<int>( unfulfilled ) == "A(x), B(x)" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "cycle detection (acyclic)" )
    {
        items.push_back( { { 0, "A" }, {"B"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"C"}, {}, {"x"} } );
        items.push_back( { { 0, "C" }, {},    {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> B --> C, x --> B --> [...]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "cycle detection (has cycles)" )
    {
        items.push_back( { { 0, "A" }, {"B"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"C"}, {}, {"x"} } );
        items.push_back( { { 0, "C" }, {"x"}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> B --> C --> x --> B [circular reference]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( not DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "an indirect unfulfilled dependency (chain reaction)" )
    {
        items.push_back( { { 0, "A" }, {"x"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"A"}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "" );
        REQUIRE( toString<int>( unfulfilled ) == "A(x), B" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }
}

TEST_CASE( "DependencyGraph transposes correctly", "[DependencyGraph]" )
{
    using DependencyGraph = DependencyGraph<int>;
    std::vector<DependencyGraph::Item> items;
    auto featureDataFactory = []( std::string ){ return DependencyGraph::Data{ 0 }; };

    SECTION( "siblings" )
    {
        items.push_back( { { 0, "A" }, {}, {}, {} } );
        items.push_back( { { 0, "B" }, {}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A, B" );
        REQUIRE( toString<int>( DependencyGraph::transpose( graph ) ) == "A, B" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "children" )
    {
        items.push_back( { { 0, "A" }, {"B"},     {}, {} } );
        items.push_back( { { 0, "B" }, {"C","D"}, {}, {} } );
        items.push_back( { { 0, "C" }, {},        {}, {} } );
        items.push_back( { { 0, "D" }, {},        {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> B --> { C, D }" );
        REQUIRE( toString<int>( DependencyGraph::transpose( graph ) ) == "C --> B --> A, D --> B --> [...]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "transposed children" )
    {
        items.push_back( { { 0, "A" }, {},    {}, {} } );
        items.push_back( { { 0, "B" }, {"A"}, {}, {} } );
        items.push_back( { { 0, "C" }, {"B"}, {}, {} } );
        items.push_back( { { 0, "D" }, {"B"}, {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "C --> B --> A, D --> B --> [...]" );
        REQUIRE( toString<int>( DependencyGraph::transpose( graph ) ) == "A --> B --> { C, D }" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "pattern O" )
    {
        items.push_back( { { 0, "A" }, {"B","C"}, {}, {} } );
        items.push_back( { { 0, "B" }, {"D"},     {}, {} } );
        items.push_back( { { 0, "C" }, {"D"},     {}, {} } );
        items.push_back( { { 0, "D" }, {},        {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> { B --> D, C --> D }" );
        REQUIRE( toString<int>( DependencyGraph::transpose( graph ) ) == "D --> { B --> A, C --> A }" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }

    SECTION( "pattern X" )
    {
        items.push_back( { { 0, "A" }, {"C"},     {}, {} } );
        items.push_back( { { 0, "B" }, {"C"},     {}, {} } );
        items.push_back( { { 0, "C" }, {"D","E"}, {}, {} } );
        items.push_back( { { 0, "D" }, {},        {}, {} } );
        items.push_back( { { 0, "E" }, {},        {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "A --> C --> { D, E }, B --> C --> [...]" );
        REQUIRE( toString<int>( DependencyGraph::transpose( graph ) ) == "D --> C --> { A, B }, E --> C --> [...]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );
    }
}

TEST_CASE( "DependencyGraph removes superflous nodes correctly", "[DependencyGraph]" )
{
    using DependencyGraph = DependencyGraph<int>;
    std::vector<DependencyGraph::Item> items;
    auto featureDataFactory = []( std::string ){ return DependencyGraph::Data{ 0 }; };

    SECTION( "pyramid keep node" )
    {
        /*
         *     A
         *    / \
         *   B   C
         *  / \ / \
         * D   E   F
         */
        items.push_back( { { 0, "A" }, {},        {}, {} } );
        items.push_back( { { 0, "B" }, {"A"},     {}, {} } );
        items.push_back( { { 0, "C" }, {"A"},     {}, {} } );
        items.push_back( { { 0, "D" }, {"B"},     {}, {} } );
        items.push_back( { { 0, "E" }, {"B","C"}, {}, {} } );
        items.push_back( { { 0, "F" }, {"C"},     {}, {} } );
        auto const [ graph, unfulfilled ] = DependencyGraph::build( items, featureDataFactory );
        REQUIRE( toString<int>( graph ) == "D --> B --> A, E --> { B --> [...], C --> A }, F --> C --> [...]" );
        REQUIRE( toString<int>( unfulfilled ) == "" );
        REQUIRE( DependencyGraph::testIsAcyclic( graph ) );

        SECTION( "bottom left" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"D"} );
            REQUIRE( toString<int>( reduced ) == "D --> B --> A" );
            REQUIRE( toString<int>( removed ) == "C, E, F" );
            REQUIRE( toString( missing ) == "" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }

        SECTION( "bottom center" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"E"} );
            REQUIRE( toString<int>( reduced ) == "E --> { B --> A, C --> A }" );
            REQUIRE( toString<int>( removed ) == "D, F" );
            REQUIRE( toString( missing ) == "" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }

        SECTION( "bottom line" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"D","E","F"} );
            REQUIRE( toString<int>( reduced ) == "D --> B --> A, E --> { B --> [...], C --> A }, F --> C --> [...]" );
            REQUIRE( toString<int>( removed ) == "" );
            REQUIRE( toString( missing ) == "" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }

        SECTION( "middle left" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"B"} );
            REQUIRE( toString<int>( reduced ) == "B --> A" );
            REQUIRE( toString<int>( removed ) == "C, D, E, F" );
            REQUIRE( toString( missing ) == "" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }

        SECTION( "middle right and non-existent" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"C","x"} );
            REQUIRE( toString<int>( reduced ) == "C --> A" );
            REQUIRE( toString<int>( removed ) == "B, D, E, F" );
            REQUIRE( toString( missing ) == "x" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }

        SECTION( "top" )
        {
            auto const [ reduced, removed, missing ] = DependencyGraph::removeSuperfluousNodes( graph, {"A"} );
            REQUIRE( toString<int>( reduced ) == "A" );
            REQUIRE( toString<int>( removed ) == "B, C, D, E, F" );
            REQUIRE( toString( missing ) == "" );
            REQUIRE( DependencyGraph::testIsAcyclic( reduced ) );
        }
    }
}

// clang-format on
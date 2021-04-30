/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Graph/Routing/Dijkstra.h>

#include <cassert>
#include <iostream>

namespace Core::Graph::Routing {

Dijkstra::PathNode::PathNode(Core::Graph::Edge edge, double cost, std::shared_ptr<PathNode> const & previous) : edge_(edge), cost_(cost), previous_(previous)
{
}

Core::Graph::Edge Dijkstra::PathNode::edge() const
{
    return this->edge_;
}

double Dijkstra::PathNode::cost() const
{
    return this->cost_;
}

PathViewImpl * Dijkstra::PathNode::previous() const
{
    return this->previous_.get();
}

Dijkstra::Dijkstra(Core::Graph::Graph const & graph) : graph_{graph}
{
}

PathView Dijkstra::operator()(Core::Graph::Node source, Core::Graph::Node destination)
{
    this->init(source);

    while (not frontier_.empty())
    {
        // if( not filterFunction_( PathView( frontier_.top().get() ) ) )
        // {
        //     auto last = this->rollback( frontier_.top() );
        //     if( last != nullptr ){
        //         frontier_.push( last );
        //         std::cerr << "Dijkstra::operator() filterfunction adding : " << PathView( last.get() ) << '\n';
        //     }
        //     std::cerr << "Dijkstra::operator() filterfunction popping : " << PathView( frontier_.top().get() ) << '\n';
        //     frontier_.pop();
        //     continue;
        // }

        if (this->reached(frontier_.top(), destination))
            return PathView(frontier_.top().get());

        auto top = frontier_.top();
        frontier_.pop();
        for (auto const & node : this->explore(top))
        {
            assert(node != nullptr);
            if (filterFunction_(PathView(node.get())))
                frontier_.push(node);
            else
                visited_.erase(node->edge_);
        }
    }
    return PathView(nullptr);
}

void Dijkstra::init(Core::Graph::Node source)
{
    frontier_ = {};
    visited_ = {};
    for (auto edge : graph_.outEdges(source))
    {
        frontier_.emplace(new PathNode(edge, costFunction_(edge), nullptr));
        visited_.insert(edge);
        auto newNode = std::make_unique<PathNode>(edge, 0.0, nullptr);
        //std::cerr << "Dijkstra::init() adding : " << PathView( newNode.get() ) << '\n';
    }
}

// std::shared_ptr<Dijkstra::PathNode> Dijkstra::rollback( std::shared_ptr<PathNode> pathNode )
// {
//     // TODO Go back to the first node with more than one out edge.
//     //      Reinsert that node into the frontier_.
//     //      Un-mark all edges on the way as visited.
//     std::cerr << "Dijkstra::rollback() rolling back : " << PathView( pathNode.get() ) << '\n';
//     while( pathNode != nullptr )
//     {
//         visited_.erase( pathNode->edge_ );
//         auto const & outEdges = graph_.outEdges( graph_.source( pathNode->edge_ ) );
//         std::cerr << "Dijkstra::rollback() id : " << pathNode->edge_.id()
//                   << ", outedges.size : " << outEdges.size() << '\n';
//         if( outEdges.size() > 1 )
//         {
//             std::cerr << "Dijkstra::rollback() returning : " << PathView( pathNode->previous_.get() ) << '\n';
//             return pathNode->previous_;
//         }
//         pathNode = pathNode->previous_;
//     }
//     std::cerr << "Dijkstra::rollback() returning : " << PathView( nullptr ) << '\n';
//     return nullptr;
// }

std::vector<std::shared_ptr<Dijkstra::PathNode>> Dijkstra::explore(std::shared_ptr<PathNode> const & parent)
{
    auto const & outEdges = graph_.outEdges(graph_.target(parent->edge()));
    auto newNodes = std::vector<std::shared_ptr<PathNode>>{};
    for (auto edge : outEdges)
    {
        if (visited_.count(edge) != 0)
            continue;
        visited_.insert(edge);
        auto cost = parent->cost_ + costFunction_(edge);
        newNodes.emplace_back(new PathNode(edge, cost, parent));
    }
    return newNodes;
}

bool Dijkstra::reached(std::shared_ptr<PathNode> const & pathNode, Core::Graph::Node graphNode)
{
    return graph_.target(pathNode->edge()) == graphNode;
}

bool operator>(std::shared_ptr<Dijkstra::PathNode> const & lhs, std::shared_ptr<Dijkstra::PathNode> const & rhs)
{
    return lhs->cost() > rhs->cost();
}

}  // namespace Core::Graph::Routing
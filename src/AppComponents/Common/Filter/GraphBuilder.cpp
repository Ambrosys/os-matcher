/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Filter/GraphBuilder.h>

#include <amblog/global.h>

#include <cassert>
#include <unordered_map>

namespace AppComponents::Common::Filter {

GraphBuilder::GraphBuilder() : Filter("GraphBuilder")
{
    setRequirements({"NodePairList", "TravelDirectionList"});
    setOptionals({});
    setFulfillments({"Graph", "GraphEdgeMap", "StreetIndexMap", "NodeMap"});
}

bool GraphBuilder::operator()(
    Types::Street::NodePairList const & nodePairList,
    Types::Street::TravelDirectionList const & travelDirectionList,
    Types::Graph::Graph & graph,
    Types::Graph::GraphEdgeMap & graphEdgeMap,
    Types::Graph::StreetIndexMap & streetIndexMap,
    Types::Graph::NodeMap & nodeMap)
{
    assert(nodePairList.size() == travelDirectionList.size());

    // TODO: std::optional only because Node is not default-constructible.
    auto streetNodeMap = std::unordered_map<size_t, std::optional<Core::Graph::Node>>{};

    auto getOrAddNode = [&](size_t const id) -> std::optional<Core::Graph::Node>
    {
        if (auto it = streetNodeMap.find(id); it != streetNodeMap.end())
            return it->second;
        else
        {
            auto node = graph.createNode();
            streetNodeMap.insert({id, node});
            nodeMap.insert({node, id});
            return node;
        }
    };

    for (size_t streetIndex = 0; streetIndex < nodePairList.size(); ++streetIndex)
    {
        auto const & nodePair = nodePairList[streetIndex];
        auto const & travelDirection = travelDirectionList[streetIndex];

        std::optional<Core::Graph::Node> sourceNode = getOrAddNode(nodePair.first);
        std::optional<Core::Graph::Node> targetNode = getOrAddNode(nodePair.second);

        Types::Graph::GraphTriplePair graphTriplePair;
        if (travelDirection == Types::Street::TravelDirection::both || travelDirection == Types::Street::TravelDirection::forwards)
        {
            auto edge = graph.addEdge(*sourceNode, *targetNode);
            graphEdgeMap.insert({edge, {streetIndex, true}});
            graphTriplePair.forwards = std::make_tuple(*sourceNode, edge, *targetNode);
        }
        if (travelDirection == Types::Street::TravelDirection::both || travelDirection == Types::Street::TravelDirection::backwards)
        {
            auto edge = graph.addEdge(*targetNode, *sourceNode);
            graphEdgeMap.insert({edge, {streetIndex, false}});
            graphTriplePair.backwards = std::make_tuple(*targetNode, edge, *sourceNode);
        }
        streetIndexMap.insert({streetIndex, graphTriplePair});
    }

    APP_LOG(noise) << graphEdgeMap.size() << " edges created";

    return true;
}

}  // namespace AppComponents::Common::Filter

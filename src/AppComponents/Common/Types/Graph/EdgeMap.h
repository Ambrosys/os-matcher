
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Graph/Graph.h>

#include <cstddef>
#include <optional>
#include <unordered_map>

namespace AppComponents::Common::Types::Graph {

struct StreetEdge
{
    size_t streetIndex;
    bool forwards;
};

using GraphTriple = std::tuple<Core::Graph::Node, Core::Graph::Edge, Core::Graph::Node>;

struct GraphTriplePair
{
    std::optional<GraphTriple> forwards;
    std::optional<GraphTriple> backwards;
};

/// Maps graph edge to street segment.
using GraphEdgeMap = std::unordered_map<Core::Graph::Edge, StreetEdge>;
/// Maps street segment to graph edge
using StreetIndexMap = std::unordered_map<size_t, GraphTriplePair>;
/// Maps graph node to street junction
using NodeMap = std::unordered_map<Core::Graph::Node, size_t>;

}  // namespace AppComponents::Common::Types::Graph

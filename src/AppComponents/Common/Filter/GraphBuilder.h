/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Graph/Graph.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>

#include <ambpipeline/Filter.h>

namespace AppComponents::Common::Filter {

class GraphBuilder : public ambpipeline::Filter
{
public:
    GraphBuilder();
    bool operator()(
        Types::Street::NodePairList const &,
        Types::Street::TravelDirectionList const &,
        Types::Graph::Graph &,
        Types::Graph::GraphEdgeMap &,
        Types::Graph::StreetIndexMap &,
        Types::Graph::NodeMap &);
};

}  // namespace AppComponents::Common::Filter

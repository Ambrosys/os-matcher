
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace AppComponents::Common::Types::Street {

/**
 * Represents a junction between two street segments.
 */
using NodePair = std::pair<size_t, size_t>;

struct NodePairList : public std::vector<NodePair>
{
};

}  // namespace AppComponents::Common::Types::Street

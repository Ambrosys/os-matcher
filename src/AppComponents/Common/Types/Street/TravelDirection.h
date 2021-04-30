
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>

namespace AppComponents::Common::Types::Street {

enum class TravelDirection {
    both,       ///< Travelling allowed in both directions
    forwards,   ///< Travelling allowed in the direction the coordinates are defined.
    backwards,  ///< Travelling allowed in reverse of the direction the coordinates are defined.
};

using TravelDirectionList = std::vector<TravelDirection>;

}  // namespace AppComponents::Common::Types::Street


/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>

namespace AppComponents::Common::Types::Street {

enum class TravelDirection { both, forwards, backwards };

using TravelDirectionList = std::vector<TravelDirection>;

}  // namespace AppComponents::Common::Types::Street

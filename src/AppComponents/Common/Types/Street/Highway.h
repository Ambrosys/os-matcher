
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <optional>
#include <vector>

namespace AppComponents::Common::Types::Street {

/**
 * See https://wiki.openstreetmap.org/wiki/Key:highway
 */
enum class HighwayType { motorway, trunk, primary, secondary, tertiary, motorway_link, trunk_link, primary_link, secondary_link, tertiary_link };

using Highway = std::optional<HighwayType>;

using HighwayList = std::vector<Highway>;

}  // namespace AppComponents::Common::Types::Street

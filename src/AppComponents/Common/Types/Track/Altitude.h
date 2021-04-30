
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

/**
 * Represents the height above ground.
 */
using Altitude = double;

struct AltitudeList : public std::vector<Altitude>
{
};

}  // namespace AppComponents::Common::Types::Track

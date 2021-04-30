
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

/**
 * Represents the velocity [m/s].
 */
using Velocity = double;

struct VelocityList : public std::vector<Velocity>
{
};

}  // namespace AppComponents::Common::Types::Track

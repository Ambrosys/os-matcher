
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>

namespace AppComponents::Common::Types::Track {

/**
 * Represents the heading (in degrees, 0 is north, clockwise direction).
 */
using Heading = double;

struct HeadingList : public std::vector<Heading>
{
};

}  // namespace AppComponents::Common::Types::Track

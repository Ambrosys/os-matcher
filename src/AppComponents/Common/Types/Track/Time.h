
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <chrono>
#include <vector>

namespace AppComponents::Common::Types::Track {

/**
 * Represents the timestamp.
 */
using Time = std::chrono::system_clock::time_point;

struct TimeList : public std::vector<Time>
{
};

}  // namespace AppComponents::Common::Types::Track

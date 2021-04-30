
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Common/Geometry/Types.h>

#include <vector>

namespace AppComponents::Common::Types::Track {

/**
 * Represents a coordinate pair.
 */
using Point = Core::Common::Geometry::Point;

struct PointList : public std::vector<Point>
{
};

}  // namespace AppComponents::Common::Types::Track

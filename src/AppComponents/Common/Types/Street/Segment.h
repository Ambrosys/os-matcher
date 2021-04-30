
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Common/Geometry/Types.h>

#include <vector>

namespace AppComponents::Common::Types::Street {

struct Segment
{
    size_t originId;
    size_t originOffset{0};  ///< In case the original segment was splitted.
    Core::Common::Geometry::LineString geometry;
};

using SegmentList = std::vector<Segment>;

}  // namespace AppComponents::Common::Types::Street

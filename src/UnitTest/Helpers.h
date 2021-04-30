/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Common/Geometry/Types.h>

#include <catch2/catch.hpp>

#include <sstream>
#include <string>

void require_close(Core::Common::Geometry::Point const & p1, Core::Common::Geometry::Point const & p2, double eps = 0.0001);

void require_close(Core::Common::Geometry::LineString const & l1, Core::Common::Geometry::LineString const & l2, double eps = 0.0001);

void assert_jsoneql(std::string const & str1, std::string const & str2);

void assert_jsoneql(std::stringstream const & ss1, std::stringstream const & ss2);

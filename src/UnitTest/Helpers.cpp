/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <UnitTest/Helpers.h>

#include <nlohmann/json.hpp>

void require_close(Core::Common::Geometry::Point const & p1, Core::Common::Geometry::Point const & p2, double eps)
{
    REQUIRE(p1.lat() == Approx(p2.lat()).epsilon(eps));
    REQUIRE(p1.lon() == Approx(p2.lon()).epsilon(eps));
}

void require_close(Core::Common::Geometry::LineString const & l1, Core::Common::Geometry::LineString const & l2, double eps)
{
    REQUIRE(l1.size() == l2.size());
    for (size_t i = 0; i < l1.size(); ++i)
    {
        require_close(l1[i], l2[i], eps);
    }
}

void assert_jsoneql(std::string const & str1, std::string const & str2)
{
    auto j1 = nlohmann::json::parse(str1);
    auto j2 = nlohmann::json::parse(str2);
    REQUIRE(j1.dump() == j2.dump());
}

void assert_jsoneql(std::stringstream const & ss1, std::stringstream const & ss2)
{
    assert_jsoneql(ss1.str(), ss2.str());
}

/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/Routing/Generic/Helper.h>

namespace AppComponents::Common::Matcher::Routing::Generic {

std::optional<size_t> findPreviousAllowed(size_t value, size_t min, std::unordered_set<size_t> const & blocklist)
{
    size_t i = value;
    while (i == value || blocklist.find(i) != blocklist.end())
        if (i > min)
            i--;
        else
            return std::nullopt;
    return i;
}

std::optional<size_t> findNextAllowed(size_t value, size_t max, std::unordered_set<size_t> const & blocklist)
{
    size_t i = value;
    while (i == value || blocklist.find(i) != blocklist.end())
        if (i < max)
            i++;
        else
            return std::nullopt;
    return i;
}

}  // namespace AppComponents::Common::Matcher::Routing::Generic

/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <pqxx/pqxx>

#include <utility>

namespace Core::Common::Postgres {

template <typename T, typename C>
std::optional<T> getOptional(C const & cell)
{
    if (!cell.is_null())
        return cell.template as<T>();
    else
        return std::nullopt;
}

template <typename T, typename C>
T getOptional(C const & cell, T defaultValue)
{
    if (!cell.is_null())
        return cell.template as<T>();
    else
        return defaultValue;
}

}  // namespace Core::Common::Postgres

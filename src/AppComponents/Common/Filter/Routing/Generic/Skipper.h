/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include <unordered_set>

namespace AppComponents::Common::Filter::Routing::Generic {

class Skipper
{
public:
    using CostFunction = std::function<double(size_t, size_t)>;
    enum class Strategy { includeEdgeCosts, excludeEdgeCosts };

    Skipper(
        size_t source,
        size_t target,
        size_t lowerBound,
        size_t upperBound,
        std::unordered_set<size_t> const & blocklist,
        CostFunction const & costFunction,
        double costLimit,
        Strategy strategy);

    bool isValid() const { return isValid_; }

    size_t source() const { return currentSource_; }

    size_t target() const { return currentTarget_; }

    double cost() const { return currentCost_; }

    std::unordered_set<size_t> skipped() const { return skipped_; }

    bool next();

private:
    size_t const lowerBound_;
    size_t const upperBound_;
    std::unordered_set<size_t> const & blocklist_;
    CostFunction const & costFunction_;
    double const costLimit_;
    Strategy const strategy_;

    bool isValid_{true};
    size_t lastSource_;
    size_t lastTarget_;
    size_t currentSource_;
    size_t currentTarget_;
    double currentCost_{0};
    std::unordered_set<size_t> skipped_;
};

class SelectiveSkipper
{
public:
    SelectiveSkipper(
        size_t source,
        size_t target,
        size_t lowerBound,
        size_t upperBound,
        std::unordered_set<size_t> const & blocklist,
        Skipper::CostFunction const & costFunction,
        double costLimit,
        Skipper::Strategy strategy);

    bool isValid() const { return selectedSkipper_ != nullptr; }

    size_t source() const { return selectedSkipper_->source(); }

    size_t target() const { return selectedSkipper_->target(); }

    std::unordered_set<size_t> skipped() const { return selectedSkipper_->skipped(); }

    bool next();

private:
    std::array<Skipper, 3> skippers_;
    Skipper * selectedSkipper_{&skippers_[0]};
};

}  // namespace AppComponents::Common::Filter::Routing::Generic

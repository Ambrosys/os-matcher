/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/Routing/Generic/Helper.h>
#include <AppComponents/Common/Matcher/Routing/Generic/Skipper.h>

#include <cassert>

namespace AppComponents::Common::Matcher::Routing::Generic {

Skipper::Skipper(
    size_t const source,
    size_t const target,
    size_t const lowerBound,
    size_t const upperBound,
    std::unordered_set<size_t> const & blocklist,
    CostFunction const & costFunction,
    double const costLimit,
    Strategy const strategy)
  : lowerBound_(lowerBound), upperBound_(upperBound), blocklist_(blocklist), costFunction_(costFunction), costLimit_(costLimit), strategy_(strategy), lastSource_(source),
    lastTarget_(target), currentSource_(source), currentTarget_(target)
{
}

bool Skipper::next()
{
    if (!isValid())
        return false;

    auto const nextSource = findPreviousAllowed(currentSource_, lowerBound_, blocklist_);
    auto const nextTarget = findNextAllowed(currentTarget_, upperBound_, blocklist_);

    double nextSourceCost = 0.0;
    double nextTargetCost = 0.0;
    bool isNextSourceValid = false;
    bool isNextTargetValid = false;
    if (nextSource)
    {
        switch (strategy_)
        {
            case Strategy::includeEdgeCosts: nextSourceCost = costFunction_(currentSource_, *nextSource); break;
            case Strategy::excludeEdgeCosts: nextSourceCost = costFunction_(lastSource_, currentSource_); break;
        }
        isNextSourceValid = currentCost_ + nextSourceCost <= costLimit_;
    }
    if (nextTarget)
    {
        switch (strategy_)
        {
            case Strategy::includeEdgeCosts: nextTargetCost = costFunction_(currentTarget_, *nextTarget); break;
            case Strategy::excludeEdgeCosts: nextTargetCost = costFunction_(lastTarget_, currentTarget_); break;
        }
        isNextTargetValid = currentCost_ + nextTargetCost <= costLimit_;
    }

    if (isNextSourceValid && (!isNextTargetValid || nextSourceCost < nextTargetCost))
    {
        skipped_.insert(currentSource_);
        lastSource_ = currentSource_;
        currentSource_ = *nextSource;
        currentCost_ += nextSourceCost;
    }
    else if (isNextTargetValid)
    {
        skipped_.insert(currentTarget_);
        lastTarget_ = currentTarget_;
        currentTarget_ = *nextTarget;
        currentCost_ += nextTargetCost;
    }
    else
        isValid_ = false;

    return isValid();
}

SelectiveSkipper::SelectiveSkipper(
    size_t const source,
    size_t const target,
    size_t const lowerBound,
    size_t const upperBound,
    std::unordered_set<size_t> const & blocklist,
    Skipper::CostFunction const & costFunction,
    double const costLimit,
    Skipper::Strategy const strategy)
  : skippers_({// forwards
               Skipper{source, target, target, upperBound, blocklist, costFunction, costLimit, strategy},
               // backwards
               Skipper{source, target, lowerBound, source, blocklist, costFunction, costLimit, strategy},
               // bidirectional
               Skipper{source, target, lowerBound, upperBound, blocklist, costFunction, costLimit, strategy}})
{
    // Call `next()` on all skippers but the first, because it is the current skipper
    // and it's `next()` gets called when `SelectiveSkipper::next()` is called.
    for (size_t i = 1; i < skippers_.size(); i++)
        skippers_[i].next();
}

bool SelectiveSkipper::next()
{
    if (!isValid())
        return false;

    assert(selectedSkipper_ != nullptr);
    selectedSkipper_->next();
    selectedSkipper_ = nullptr;

    // select skipper with minimum cost
    double minCost = 0.0;
    for (auto & skipper : skippers_)
        if (skipper.isValid() && ((selectedSkipper_ == nullptr) || minCost > skipper.cost()))
        {
            minCost = skipper.cost();
            selectedSkipper_ = &skipper;
        }

    return isValid();
}

}  // namespace AppComponents::Common::Matcher::Routing::Generic

/*
 * SPDX-FileCopyrightText: © 2017 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <chrono>
#include <ostream>
#include <string>

namespace Generic {

class Progress
{
public:
    Progress(std::string const & text, uint64_t ticks) : text_(text), totalTicks_(ticks), ticksOccured_(0), beginTime_(std::chrono::steady_clock::now()) {}

    void tick();
    void updateTotal(uint64_t ticks) { totalTicks_ = ticks; }

private:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float, Clock::period>;

    void output(std::ostream & os, Duration duration);

    std::string text_;
    std::uint64_t totalTicks_;
    std::uint64_t ticksOccured_;
    Clock::time_point const beginTime_;
};

}  // namespace Generic

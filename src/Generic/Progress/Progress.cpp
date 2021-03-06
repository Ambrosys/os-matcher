/*
 * SPDX-FileCopyrightText: © 2017 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Generic/Progress/Progress.h>

#include <cassert>
#include <iomanip>
#include <iostream>

namespace Generic {

void Progress::tick()
{
    ticksOccured_++;

    //if( ticksOccured_ % 10 != 0 )
    //    return;

    assert(ticksOccured_ > 0);

    Duration time_done = Clock::now() - beginTime_;
    Duration time_total = time_done / ((float)ticksOccured_ / totalTicks_);
    Duration time_left = time_total - time_done;

    std::cout << "\33[2K\r" << text_ << ": " << ticksOccured_ << "/" << totalTicks_;

    std::streamsize streamsize = std::cout.precision();
    std::cout << " | " << std::fixed << std::setprecision(2) << ticksOccured_ / (std::chrono::duration_cast<std::chrono::milliseconds>(time_done).count() / 1000.0 / 60.0)
              << "/min";
    std::cout << std::defaultfloat;
    std::cout.precision(streamsize);

    std::cout << " | ";
    output(std::cout, time_done);
    std::cout << '/';
    output(std::cout, time_total);
    std::cout << " | ETA ";
    output(std::cout, time_left);
    std::cout << std::flush;
}

void Progress::output(std::ostream & os, Duration duration)
{
    auto h = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(duration - h);
    auto s = std::chrono::duration_cast<std::chrono::seconds>(duration - h - m);
    os << h.count() << ':' << std::setw(2) << std::setfill('0') << m.count() << ':' << std::setw(2) << std::setfill('0') << s.count() << 'h';
}

}  // namespace Generic

/**
 * Copyright 2021 Michel Kremer (kremi151)
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "frame_executor.h"

#if HAS_STD_THIS_THREAD
#include <thread>
#else
#include <ctime>

void FunkyBoy::Util::FrameExecutor::sleepms(unsigned long milli) {
    clock_t end_time = clock() + milli * CLOCKS_PER_SEC/1000;
    while (clock() < end_time) {
        // No-op
    }
}
#endif

using namespace FunkyBoy::Util;

using hrclock = std::chrono::high_resolution_clock;

FrameExecutor::FrameExecutor(std::function<void(void)> func, double fps)
    : func(std::move(func))
    , durationPerFrame(1000.0 / fps)
{
}

void FrameExecutor::operator()() {
    auto frameStart = hrclock::now();
    func();
    auto timeSinceFrameStart = std::chrono::duration_cast<std::chrono::milliseconds>(hrclock::now() - frameStart).count();
    auto delay = (int)durationPerFrame - timeSinceFrameStart;

#if HAS_STD_THIS_THREAD
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
#else
    sleepms(delay);
#endif
}

#endif

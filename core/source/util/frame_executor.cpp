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

#ifdef FB_FRAME_EXECUTOR_SUPPORTED

#if HAS_STD_THIS_THREAD
#include <thread>
using duration_t = std::chrono::milliseconds;
#define ONE_SECOND 1000.0
#elif __PSP__
#include <pspkernel.h>
#define ONE_SECOND 1000000.0
#elif HAS_UNISTD_USLEEP
#include <unistd.h>
using duration_t = std::chrono::microseconds;
#define ONE_SECOND 1000000.0
#endif

using namespace FunkyBoy::Util;

using hrclock = std::chrono::high_resolution_clock;

FrameExecutor::FrameExecutor(std::function<void(void)> func, double fps)
    : func(std::move(func))
    , durationPerFrame(ONE_SECOND / fps)
{
}

void FrameExecutor::operator()() {
#ifdef __PSP__
    auto frameStartMicros = sceKernelGetSystemTimeLow();
#else
    auto frameStart = hrclock::now();
#endif

    func();

#ifdef __PSP__
    auto delay = sceKernelGetSystemTimeLow() - frameStartMicros;
#else
    auto timeSinceFrameStart = std::chrono::duration_cast<duration_t>(hrclock::now() - frameStart).count();
    auto delay = (int)durationPerFrame - timeSinceFrameStart;
#endif

#if HAS_STD_THIS_THREAD
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
#elif __PSP__
    if (delay > 0) {
        sceKernelDelayThread(delay);
    }
#elif HAS_UNISTD_USLEEP
    if (delay > 0) {
        usleep(delay);
    }
#endif
}

#endif
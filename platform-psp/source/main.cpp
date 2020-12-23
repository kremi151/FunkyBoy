/**
 * Copyright 2020 Michel Kremer (kremi151)
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

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <util/typedefs.h>
#include "callback.h"

PSP_MODULE_INFO(FB_NAME, 0, FB_VERSION_MAJOR, FB_VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

int main() {
    // pspDebugScreenInit();
    setupExitCallback();
    while (isRunning()) {
        // pspDebugScreenSetXY(0, 0);
    }

    sceKernelExitGame();
    return 0;
}
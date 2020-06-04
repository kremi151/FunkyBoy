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

#ifndef FUNKYBOY_CORE_DEBUG_H
#define FUNKYBOY_CORE_DEBUG_H

#ifdef FB__DEBUG

// Uncomment to enable step-by-step breakpoints
// #define FB_DEBUG_STEPS

#include <cstdio>

#define debug_print(fmt, ...) \
        do { fprintf(stdout, fmt, __VA_ARGS__); } while (0)

#define debug_print_with_line(fmt, ...) \
        do { fprintf(stdout, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#else

#define debug_print(fmt, ...)
#define debug_print_with_line(fmt, ...)

#endif

#endif //FUNKYBOY_CORE_DEBUG_H

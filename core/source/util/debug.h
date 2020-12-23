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

#ifndef FB_DEBUG_LOG_LEVEL
#define FB_DEBUG_LOG_LEVEL 2
#endif

#ifdef __PSP__
#include <pspdebug.h>
#define __fb_printf(fmt, ...) scDebugScreenPrintf(fmt, __VA_ARGS__)
#else
#define __fb_printf(fmt, ...) fprintf(stdout, fmt, __VA_ARGS__)
#endif

// Uncomment to enable step-by-step breakpoints
// #define FB_DEBUG_STEPS

#include <cstdio>

#if defined(FB_DEBUG) && FB_DEBUG_LOG_LEVEL >= 4
#define debug_print_4(fmt, ...) \
        do { __fb_printf(fmt, __VA_ARGS__); } while (0)
#else
#define debug_print_4(fmt, ...) ((void)0)
#endif

#if defined(FB_DEBUG) && FB_DEBUG_LOG_LEVEL >= 2
#define debug_print_2(fmt, ...) \
        do { __fb_printf(fmt, __VA_ARGS__); } while (0)
#else
#define debug_print_2(fmt, ...) ((void)0)
#endif

#ifdef FB_DEBUG

#define debug_print_with_line(fmt, ...) \
        do { __fb_printf("%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#define debug_break() while (getchar() == '\0')

#else

#define debug_print_with_line(fmt, ...) ((void)0)
#define debug_break() ((void)0)

#endif



#endif //FUNKYBOY_CORE_DEBUG_H

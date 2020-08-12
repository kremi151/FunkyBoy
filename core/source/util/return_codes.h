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

#ifndef FB_CORE_RETURN_CODES_H
#define FB_CORE_RETURN_CODES_H

#define FB_RET_SUCCESS 1
#define FB_RET_SKIPPED (1 << 1)
#define FB_RET_NEW_SCANLINE (1 << 2)
#define FB_RET_NEW_FRAME (1 << 3)

#endif //FB_CORE_RETURN_CODES_H

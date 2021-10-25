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

#ifndef FB_CORE_UTIL_SERIALIZATION_H
#define FB_CORE_UTIL_SERIALIZATION_H

#include <iostream>

#define FB_DECLARE_SERIALIZATION_ESTIMATABLE() \
static size_t serializationSize();

#define FB_DECLARE_SERIALIZATION(...) \
FB_DECLARE_SERIALIZATION_ESTIMATABLE() \
void serialize(std::ostream &ostream) const __VA_ARGS__; \
void deserialize(std::istream &istream) __VA_ARGS__;

#define FB_DECLARE_SERIALIZATION_VIRTUAL(...) \
virtual void serialize(std::ostream &ostream) const __VA_ARGS__; \
virtual void deserialize(std::istream &istream) __VA_ARGS__;

#endif //FB_CORE_UTIL_SERIALIZATION_H

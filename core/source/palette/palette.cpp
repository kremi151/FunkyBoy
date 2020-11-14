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

#include "palette.h"

using namespace FunkyBoy::Palette;

color::color(): red(0), green(0), blue(0) {
}

color::color(FunkyBoy::u8 red, FunkyBoy::u8 green, FunkyBoy::u8 blue): red(red), green(green), blue(blue) {
}

void palette::setColor(int index, FunkyBoy::u8 red, FunkyBoy::u8 green, FunkyBoy::u8 blue) {
    colors[index] = color(red, green, blue);
}

const color& palette::operator[](int index) const {
    return colors[index];
}

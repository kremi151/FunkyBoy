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

#ifndef FB_SDL_PALETTE_H
#define FB_SDL_PALETTE_H

#include <util/typedefs.h>

namespace FunkyBoy::Palette {

    class color {
    private:
        u8 red, green, blue;
    public:
        color();
        color(u8 red, u8 green, u8 blue);

        inline u8 getRed() const {
            return red;
        }

        inline u8 getGreen() const {
            return green;
        }

        inline u8 getBlue() const {
            return blue;
        }

    };

    class palette {
    private:
        color colors[4]{};
    public:
        palette() = default;

        void setColor(int index, u8 red, u8 green, u8 blue);
        const color &operator[] (int index) const;
    };

}

#endif //FB_SDL_PALETTE_H

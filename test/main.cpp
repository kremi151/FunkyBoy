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

#include <acacia.h>
#include <memory/memory.h>
#include <memory>

TEST(test16BitReadWrite) {
    std::shared_ptr<FunkyBoy::Cartridge> cartridge(new FunkyBoy::Cartridge);
    FunkyBoy::Memory memory(cartridge);

    memory.write16BitsTo(0xC000, 0x12, 0x34);
    int val = memory.read16BitsAt(0xC000);
    assertEquals(0x1234, val);

    memory.write16BitsTo(0xC002, 0x1234);
    val = memory.read16BitsAt(0xC002);
    assertEquals(0x1234, val);
}

int main() {
    return acacia::Registry::instance().runTests();
}
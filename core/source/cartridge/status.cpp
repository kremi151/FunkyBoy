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

#include "status.h"

std::string FunkyBoy::getCartridgeStatusDescription(CartridgeStatus status) {
    switch (status) {
        case CartridgeStatus::NoROMLoaded:
            return "No ROM was loaded";
        case CartridgeStatus::ROMFileNotReadable:
            return "ROM file could not be read";
        case CartridgeStatus::ROMParseError:
            return "Selected file is either corrupted or not a Game boy ROM";
        case CartridgeStatus::ROMTooBig:
            return "Selected file is too big";
        case CartridgeStatus::ROMSizeMismatch:
            return "The size of the ROM file does not match its header information. This usually indicated that the file is corrupted.";
        case CartridgeStatus::ROMUnsupportedMBC:
            return "The ROM file uses an unsupported memory banking chip (MBC)";
        case CartridgeStatus::RAMSizeUnsupported:
            return "The ROM file requires a specific RAM size which is not supported";
        case CartridgeStatus::Loaded:
            return "The ROM file has been loaded successfully";
        default:
            return "Unknown status";
    }
}
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

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <ui/native_ui.h>

using namespace FunkyBoy::SDL;

void NativeUI::init(int argc, char **argv) {
}

void NativeUI::deinit() {
}

void NativeUI::selectFiles(SDL_Window *window, const char *title, const std::vector<file_type> &types, bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles) {
    @autoreleasepool {
        NSOpenPanel *openDlg = [NSOpenPanel openPanel];

        NSMutableArray<NSString*> *fileTypeFilter = [[NSMutableArray alloc] init];
        for (const file_type &type : types) {
            [fileTypeFilter addObject:[NSString stringWithUTF8String:type.extension.c_str()]];
        }

        [openDlg setCanChooseFiles:YES];
        [openDlg setAllowedFileTypes:fileTypeFilter];
        [openDlg setAllowsMultipleSelection:allowMultiple];

        if ([openDlg runModal] != NSModalResponseOK) {
            return;
        }

        NSArray<NSURL*> *files = [openDlg URLs];
        for (id url in files) {
            NSString *path = [url path];
            outFiles.push_back([path UTF8String]);
        }
    }
}

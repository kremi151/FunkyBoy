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

#include <ui/file_picker.h>

using namespace FunkyBoy::SDL;

FunkyBoy::fs::path FilePicker::selectROM() {
    NSOpenPanel *openDlg = [NSOpenPanel openPanel];
    NSArray<NSString*> *fileTypeFilter = @[@"gb", @"bin"];

    [openDlg setCanChooseFiles:YES];
    [openDlg setAllowedFileTypes:fileTypeFilter];
    [openDlg setAllowsMultipleSelection:NO];

    if ([openDlg runModal] == NSModalResponseOK) {
        NSArray<NSURL*> *files = [openDlg URLs];
        if ([files count] > 0) {
            NSString *path = [[files objectAtIndex:0] path];
            return [path UTF8String];
        }
    }
    return "";
}

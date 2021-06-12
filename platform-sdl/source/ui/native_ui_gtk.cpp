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

#ifdef FB_USE_GTK

#include "native_ui.h"
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <SDL_syswm.h>

#include <exception/state_exception.h>

using namespace FunkyBoy::SDL;

namespace FunkyBoy::SDL {
    inline void flushGtkEvents() {
        while(gtk_events_pending()) {
            gtk_main_iteration();
        }
    }
}

void NativeUI::init(int &argc, char **argv) {
    XInitThreads();
    gtk_init(&argc, &argv);
}

void NativeUI::selectFiles(SDL_Window *window, const char *title, const std::vector<file_type> &types,
                           bool allowMultiple, std::vector<FunkyBoy::fs::path> &outFiles) {
    /*SDL_SysWMinfo wmi = {0};
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        fprintf(stderr, "SDL_GetWindowWMInfo error %s : %lu\n", SDL_GetError(), wmi.info.x11.window);
    }*/

    GtkWidget *widget = gtk_file_chooser_dialog_new(
            title,
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT,
            NULL
            );

    for (const auto &type : types) {
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, type.description.c_str());
        gtk_file_filter_add_pattern(filter, ("*." + type.extension).c_str());
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widget), filter);
    }

    if (allowMultiple) {
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(widget), TRUE);
    }

    auto response = gtk_dialog_run(GTK_DIALOG(widget));
    if (response != GTK_RESPONSE_ACCEPT) {
        goto gtk_quit_file_picker;
    }

    if (allowMultiple) {
        auto files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(widget));
        for (auto file = files ; file ; file = file->next) {
            std::string pathStr = static_cast<char*>(file->data);
            outFiles.emplace_back(pathStr);
            g_free(file->data);
        }
        g_slist_free(files);
    } else {
        auto file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
        std::string pathStr = static_cast<char*>(file);
        outFiles.emplace_back(pathStr);
        g_free(file);
    }

gtk_quit_file_picker:
    flushGtkEvents();
    gtk_widget_destroy(widget);
    flushGtkEvents();
}

void NativeUI::showAlert(SDL_Window*, AlertType type, const char *title, const char *message) {
    GtkMessageType alertType;
    switch (type) {
        case AlertType::Warning:
            alertType = GTK_MESSAGE_WARNING;
            break;
        case AlertType::Error:
            alertType = GTK_MESSAGE_ERROR;
            break;
        default:
            alertType = GTK_MESSAGE_INFO;
            break;
    }
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    auto dialog = gtk_message_dialog_new (NULL,
                                     flags,
                                     alertType,
                                     GTK_BUTTONS_CLOSE,
                                     "%s\n%s", title, message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void NativeUI::deinit() {
}

#endif
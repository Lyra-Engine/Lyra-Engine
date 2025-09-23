#pragma once

#ifndef LYRA_LIBRARY_APPKIT_FILE_MANAGER_H
#define LYRA_LIBRARY_APPKIT_FILE_MANAGER_H

#include <Lyra/Common/Path.h>
#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct FileManager
    {
    public:
        explicit FileManager(const Path& root);

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
        void show_breadcrumb();
        void show_dir_files();
        void show_context_menu();
        void show_new_file_dialog();
        void show_new_folder_dialog();

    private:
        Path root;
        Path curr;

        bool show_new_file_modal   = false;
        bool show_new_folder_modal = false;

        float icon_size = 128.0f;
        float padding   = 16.0f;
    };
} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_FILE_MANAGER_H

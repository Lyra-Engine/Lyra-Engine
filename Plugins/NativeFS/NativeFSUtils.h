#pragma once

#ifndef LYRA_PLUGIN_NATIVE_FS_UTILS_H
#define LYRA_PLUGIN_NATIVE_FS_UTILS_H

#include <mutex>
#include <atomic>
#include <fstream>
#include <filesystem>

#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/Common/Container.h>
#include <Lyra/FileIO/FSAPI.h>

using namespace lyra;

namespace fs = std::filesystem;

struct NativeMount
{
    String   vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    fs::path root;     // real OS directory root
    uint     priority; // higher value = searched earlier
    uint     mount_id;
};

struct NativeFile
{
    Own<std::ifstream> ifs; // used when opened for read
    fs::path           path;
};

struct NativeFSLoader
{
    Vector<NativeMount>            mounts;
    HashMap<uint, Own<NativeFile>> files;
    std::atomic<uint>              file_index  = 0;
    std::atomic<uint>              mount_index = 0;
    std::mutex                     mounts_mutex;
    std::mutex                     files_mutex;
};

#endif // LYRA_PLUGIN_NATIVE_FS_UTILS_H

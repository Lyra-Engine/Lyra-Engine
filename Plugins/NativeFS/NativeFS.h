#pragma once

#ifndef LYRA_PLUGIN_NATIVE_FS_H
#define LYRA_PLUGIN_NATIVE_FS_H

#include <fstream>
#include <filesystem>

#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/FileIO/FSAPI.h>

using namespace lyra;

namespace fs = std::filesystem;

struct NativeMountPoint
{
    String   vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    fs::path root;     // real OS directory root
    uint32_t priority; // higher value = searched earlier
    uint32_t mount_id;
};

struct NativeFileHandleData
{
    Own<std::ifstream> ifs; // used when opened for read
    fs::path           path;
};

inline bool sort_mount_points(const NativeMountPoint& a, const NativeMountPoint& b)
{
    // sort based on priority
    if (a.priority != b.priority)
        return a.priority > b.priority;

    // longer vpath first as tiebreaker
    return a.vpath.size() > b.vpath.size();
}

#endif // LYRA_PLUGIN_NATIVE_FS_H

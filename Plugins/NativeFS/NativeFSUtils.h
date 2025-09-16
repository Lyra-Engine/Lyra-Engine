#pragma once

#ifndef LYRA_PLUGIN_NATIVE_FS_UTILS_H
#define LYRA_PLUGIN_NATIVE_FS_UTILS_H

#include <fstream>
#include <filesystem>

#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>
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

auto normalize_vpath(FSPath vpath) -> String;
bool strip_prefix(String& path, const String& prefix);
bool sort_mount_points(const NativeMount& a, const NativeMount& b);

#endif // LYRA_PLUGIN_NATIVE_FS_UTILS_H

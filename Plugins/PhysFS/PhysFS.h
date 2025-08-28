#pragma once

#ifndef LYRA_PLUGIN_PHYS_FS_H
#define LYRA_PLUGIN_PHYS_FS_H

#include <physfs.h>
#include <filesystem>

#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/FileIO/FSAPI.h>

using namespace lyra;

namespace fs = std::filesystem;

// custom deleter for PHYSFS_File
struct PhysFSFileDeleter
{
    void operator()(PHYSFS_File* f) const
    {
        if (f) PHYSFS_close(f);
    }
};

using PhysFSFilePtr = std::unique_ptr<PHYSFS_File, PhysFSFileDeleter>;

struct PhysMountPoint
{
    String   vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    fs::path root;     // real OS directory root
    uint32_t priority; // higher value = searched earlier
    uint32_t mount_id;
};

struct PhysFileHandleData
{
    PhysFSFilePtr pfile; // used when opened via PhysicsFS (read-only)
};

inline bool sort_mount_points(const PhysMountPoint& a, const PhysMountPoint& b)
{
    // sort based on priority
    if (a.priority != b.priority)
        return a.priority > b.priority;

    // longer vpath first as tiebreaker
    return a.vpath.size() > b.vpath.size();
}

#endif // LYRA_PLUGIN_PHYS_FS_H

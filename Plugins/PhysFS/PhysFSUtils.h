#pragma once

#ifndef LYRA_PLUGIN_PHYS_FS_H
#define LYRA_PLUGIN_PHYS_FS_H

#include <physfs.h>

#include <Lyra/Common/Path.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/FileIO/FSAPI.h>

using namespace lyra;

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
    String vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    Path   root;     // real OS directory root
    uint   priority; // higher value = searched earlier
    uint   mount_id;
};

struct PhysFile
{
    PhysFSFilePtr pfile; // used when opened via PhysicsFS (read-only)
};

bool sort_mount_points(const PhysMountPoint& a, const PhysMountPoint& b);

#endif // LYRA_PLUGIN_PHYS_FS_H

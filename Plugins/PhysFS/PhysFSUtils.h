#pragma once

#include <Lyra/Common/Path.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Container.h>
#include <Lyra/FileIO/FSTypes.h>
#include <Lyra/Common/Stdint.h>

#include <physfs.h>

using namespace lyra;

// custom deleter for PHYSFS_File
struct PhysFSFileDeleter
{
    void operator()(PHYSFS_File* f) const
    {
        if (f) PHYSFS_close(f);
    }
};

using PhysFSFilePtr = Own<PHYSFS_File, PhysFSFileDeleter>;

struct PhysFile
{
    PhysFSFilePtr pfile;
};

struct PhysMountPoint
{
    String vpath;
    Path   root;
    uint   priority = 0;
    uint   mount_id = 0;
};

struct PhysFSLoader
{
    Vector<PhysMountPoint>       mounts;
    HashMap<uint, Own<PhysFile>> files;
    std::atomic<uint>            file_index  = 0;
    std::atomic<uint>            mount_index = 0;
    std::mutex                   mounts_mutex;
    std::mutex                   files_mutex;
};

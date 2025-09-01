#pragma once

#ifndef LYRA_LIBRARY_AMS_TYPES_H
#define LYRA_LIBRARY_AMS_TYPES_H

#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/FileIO/FSEnums.h>
#include <Lyra/FileIO/FSUtils.h>
#include <Lyra/Assets/AMSAPI.h>

namespace lyra
{
    struct AssetImporter
    {
    };

    struct AssetLoader
    {
        // AssetType needs to define the following:
        // 1. a list of suffixes that it knows how to process
        // 2. AssetImporter +
        // 3. AssetLoader
        template <typename AssetType>
        static void register_asset()
        {
        }
    };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_TYPES_H

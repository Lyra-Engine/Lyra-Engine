#pragma once

#ifndef LYRA_LIBRARY_AMS_UTILS_H
#define LYRA_LIBRARY_AMS_UTILS_H

#include <Lyra/Common/UUID.h>
#include <Lyra/Common/GUID.h>
#include <Lyra/Common/Path.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/FileIO/FSTypes.h>
#include <Lyra/Assets/AMSEnums.h>

namespace lyra
{

    struct RawAssetHandle
    {
        GUID guid;

        FORCE_INLINE bool valid() const { return guid != 0ull; }
    };

    template <typename AssetType>
    struct AssetHandle : RawAssetHandle
    {
        static constexpr UUID type_uuid = AssetType::uuid;
    };

    struct AMSImportDescriptor
    {
        // directory where user places assets
        Path asset_path;

        // directory where asset caches are generated
        Path cache_path;
    };

    struct AMSLoaderDescriptor
    {
        // file loader for loading metadata
        FileLoaderAPI* metadata;

        // file loader for loading actual asset
        FileLoaderAPI* assets;
    };

    struct AMSDescriptor
    {
        // development only
        AMSImportDescriptor importer;

        // both development + runtime
        AMSLoaderDescriptor loader;

        // watch asset directory changes
        bool watch;

        // number of threads for asset processing/loading
        uint workers;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_UTILS_H

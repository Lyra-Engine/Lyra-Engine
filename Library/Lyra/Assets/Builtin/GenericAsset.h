#pragma once

#ifndef LYRA_LIBRARY_AMS_GENERIC_ASSET_H
#define LYRA_LIBRARY_AMS_GENERIC_ASSET_H

#include <Lyra/Common/JSON.h>

namespace lyra
{
    // Dummy asset process implementation,
    // which does absolutely nothing except for saving the asset path in metadata.
    // This is suitable for simple assets that do not need an importing process.
    template <typename AssetType>
    JSON process_asset(AssetManager* manager, FSPath source_path, FSPath target_path)
    {
        (void)target_path;

        JSON metadata    = {};
        metadata["path"] = source_path;
        return metadata;
    }

    // Simple asset unload implementation, which only uses C++ pointer deletion.
    template <typename AssetType>
    void unload_asset(void* asset)
    {
        auto typed_asset = reinterpret_cast<AssetType*>(asset);
        delete typed_asset;
    }
} // namespace lyra

#endif // LYRA_LIBRARY_AMS_GENERIC_ASSET_H

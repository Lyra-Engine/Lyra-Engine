#pragma once

#ifndef LYRA_LIBRARY_AMS_TEXT_ASSET_H
#define LYRA_LIBRARY_AMS_TEXT_ASSET_H

#include <Lyra/Common/UUID.h>
#include <Lyra/Common/String.h>
#include <Lyra/Assets/AMSAPI.h>
#include <Lyra/FileIO/FSTypes.h>

namespace lyra
{
    struct TextAsset
    {
        // uuid: to let asset server know how to reference this type of asset
        static constexpr UUID uuid = make_uuid("394a204e-8622-11f0-8de9-0242ac120002");

        // extensions: to let assert server what extensions to look for
        static constexpr InitList<CString> extensions = {".txt"};

        // loader: to let asset server know how to load this type of asset
        static auto loader() -> AssetLoaderAPI*;

        // text content
        String content;
    };

    // struct TextLoader
    // {
    //     static constexpr auto get_api_name() -> CString
    //     {
    //         return "TextLoader";
    //     }
    //
    //     static auto load(FileLoaderAPI* api, VFSPath path) -> void*
    //     {
    //         auto loader  = FileLoader(api);
    //         auto content = loader.read(path);
    //         return new TextAsset{String(content.begin(), content.end())};
    //     }
    //
    //     static auto api() -> AssetLoaderAPI
    //     {
    //         AssetLoaderAPI api;
    //         api.get_api_name = TextLoader::get_api_name;
    //         api.load         = TextLoader::load;
    //         return api;
    //     }
    // };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_TEXT_ASSET_H

#pragma once

#ifndef LYRA_LIBRARY_AMS_API_H
#define LYRA_LIBRARY_AMS_API_H

#include <Lyra/Common/UUID.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/Common/JSON.h>
#include <Lyra/FileIO/FSAPI.h>
#include <Lyra/Assets/AMSUtils.h>

namespace lyra
{
    struct AssetImporterAPI
    {
        // api name
        CString (*get_api_name)();

        // configure asset specific options via JSON object
        void (*configure)(const JSON& options);

        // process assets and save to specified path (if needed),
        // return the metadata object for this imported asset
        JSON (*process)(FSPath content_path, FSPath generated_path);
    };

    struct AssetLoaderAPI
    {
        // api name
        CString (*get_api_name)();

        // load the asset and returns an int-based handle.
        // note this is untyped, we will need to attach type later.
        void* (*load)(FileLoaderAPI* loader, VFSPath path);
    };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_API_H

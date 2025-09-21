#pragma once

#ifndef LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H
#define LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H

#include <Lyra/Assets/AMSTypes.h>
#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    // AssetManager is only a wrapper around AssetServer.
    // It is created to used with AppKit's style of data
    // handling and event binding.
    struct AssetManager
    {
    public:
        explicit AssetManager(const AMSDescriptor& descriptor);

        void bind(Application& app);

        void update();

    private:
        AssetServer ams;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H

#pragma once

#ifndef LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H
#define LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H

#include <Lyra/Assets/AMSTypes.h>
#include <Lyra/Appkit/AppTypes.h>

namespace lyra
{
    struct AssetManager
    {
    public:
        explicit AssetManager(const AMSDescriptor& descriptor) : ams(descriptor) {}
        virtual ~AssetManager() = default;

        void bind(Application& app)
        {
            // save asset manager into blackboard
            app.get_blackboard().add<AssetServer*>(&ams);

            // bind asset manager events
            app.bind<AppEvent::UPDATE>(&AssetManager::update, this);
        }

        void update()
        {
            // do nothing for now,
            // we will do some asset event polling later
        }

    private:
        AssetServer ams;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_ASSET_MANAGER_H

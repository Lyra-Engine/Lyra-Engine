#include <Lyra/Engine/System/AssetManager.h>

using namespace lyra;

AssetManager::AssetManager(const AMSDescriptor& descriptor) : ams(descriptor)
{
    // do nothing
}

void AssetManager::bind(Application& app)
{
    // save asset manager into blackboard
    app.get_blackboard().add<AssetServer*>(&ams);

    // bind asset manager events
    app.bind<AppEvent::UPDATE>(&AssetManager::update, this);
}

void AssetManager::update()
{
    // do nothing for now,
    // we will do some asset event polling later
}

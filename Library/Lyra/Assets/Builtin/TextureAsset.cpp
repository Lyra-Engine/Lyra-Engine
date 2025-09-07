#include <Lyra/Common/Macros.h>
#include <Lyra/Assets/Builtin/TextureAsset.h>

using namespace lyra;

AssetHandlerAPI TextureAsset::handler()
{
    UNIMPLEMENTED("TextureAsset::handler");

    auto api    = AssetHandlerAPI{};
    api.load    = nullptr;
    api.unload  = nullptr;
    api.process = nullptr;
    return api;
}

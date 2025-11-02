#include <Lyra/Common/Plugin.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Engine/TextureAsset.h>

using namespace lyra;

using TexturePlugin = Plugin<AssetHandlerAPI>;

static Own<TexturePlugin> TEXTURE_PLUGIN;

AssetHandlerAPI TextureAsset::handler()
{
    if (!TEXTURE_PLUGIN)
        TEXTURE_PLUGIN = std::make_unique<TexturePlugin>("lyra-ktx");

    return *TEXTURE_PLUGIN->get_api();
}

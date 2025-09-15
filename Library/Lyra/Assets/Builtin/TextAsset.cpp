#include <Lyra/Assets/Builtin/TextAsset.h>
#include <Lyra/Assets/Builtin/GenericAsset.h>

using namespace lyra;

static void* load_text_asset(const FileLoader& loader, const JSON& metadata)
{
    auto path    = metadata["path"].template get<String>();
    auto content = loader.read<char>(path.c_str());
    return new TextAsset{String(content.begin(), content.end())};
}

AssetHandlerAPI TextAsset::handler()
{
    auto api    = AssetHandlerAPI{};
    api.load    = load_text_asset;
    api.unload  = unload_asset<TextAsset>;
    api.process = process_asset<TextAsset>;
    return api;
}

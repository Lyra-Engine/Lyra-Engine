#include <Lyra/Assets/Builtin/JsonAsset.h>
#include <Lyra/Assets/Builtin/GenericAsset.h>

using namespace lyra;

static void* load_json_asset(FileLoader* loader, const JSON& metadata)
{
    auto path    = metadata["path"].template get<String>();
    auto content = loader->read<char>(path.c_str());
    return new JsonAsset{JSON::parse(content.begin(), content.end())};
}

AssetHandlerAPI JsonAsset::handler()
{
    auto api    = AssetHandlerAPI{};
    api.load    = load_json_asset;
    api.unload  = unload_asset<JsonAsset>;
    api.process = process_asset<JsonAsset>;
    return api;
}

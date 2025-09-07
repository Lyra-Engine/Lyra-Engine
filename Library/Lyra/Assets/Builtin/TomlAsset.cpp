#include <Lyra/Assets/Builtin/TomlAsset.h>
#include <Lyra/Assets/Builtin/GenericAsset.h>

using namespace lyra;

static void* load_toml_asset(FileLoaderAPI* api, const JSON& metadata)
{
    auto path    = metadata["path"].template get<String>();
    auto loader  = FileLoader(api);
    auto content = loader.read<char>(path.c_str());
    auto view = StringView(content.data(), content.size() - 1);
    return new TomlAsset{toml::parse(view)};
}

AssetHandlerAPI TomlAsset::handler()
{
    auto api    = AssetHandlerAPI{};
    api.load    = load_toml_asset;
    api.unload  = unload_asset<TomlAsset>;
    api.process = process_asset<TomlAsset>;
    return api;
}

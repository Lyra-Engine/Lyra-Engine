// library headers
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Assets/AMSAPI.h>

using namespace lyra;

LYRA_EXPORT auto prepare() -> void
{
    // do nothing
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> AssetHandlerAPI
{
    auto api = AssetHandlerAPI{};
    return api;
}

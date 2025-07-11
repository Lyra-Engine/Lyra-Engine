#ifndef LYRA_TESTLIB_HELPER_COMMON_H
#define LYRA_TESTLIB_HELPER_COMMON_H

#include <Lyra/Common.hpp>
#include <Lyra/Render.hpp>
#include <Lyra/Window.hpp>

using namespace lyra;
using namespace lyra::wsi;
using namespace lyra::rhi;

template <typename T>
T align(T value, T alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

#endif // LYRA_TESTLIB_HELPER_COMMON_H

#pragma once

#ifndef LYRA_LIBRARY_COMMON_FUNCTION_H
#define LYRA_LIBRARY_COMMON_FUNCTION_H

#include <utility>

namespace lyra
{

    template <typename F>
    decltype(auto) execute(F&& fn)
    {
        return std::forward<F>(fn)();
    }

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_FUNCTION_H

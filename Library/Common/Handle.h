#ifndef LYRA_LIBRARY_COMMON_HANDLE_H
#define LYRA_LIBRARY_COMMON_HANDLE_H

#include <cstdint>

namespace lyra
{

    template <typename E, E TYPE, typename T = uint32_t>
    struct Handle
    {
        static constexpr E type = TYPE;

        T value;

        Handle() : value(T(-1)) {}

        Handle(T value) : value(value) {}

        bool valid() const { return value != T(-1); }
    };
} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_HANDLE_H

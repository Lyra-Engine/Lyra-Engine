#ifndef LYRA_LIBRARY_COMMON_BITFLAGS_H
#define LYRA_LIBRARY_COMMON_BITFLAGS_H

#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
struct BitFlags
{
    using U = std::underlying_type_t<T>;

    U value;

    BitFlags() : value(U(0)) {}

    BitFlags(U value) : value(value) {}

    BitFlags(T value) : value(value) {}

    friend BitFlags operator~(BitFlags lhs)
    {
        return BitFlags(~lhs.value);
    }

    friend BitFlags operator|(BitFlags lhs, BitFlags rhs)
    {
        return BitFlags(lhs.value | rhs.value);
    }

    friend BitFlags operator&(BitFlags lhs, BitFlags rhs)
    {
        return BitFlags(lhs.value & rhs.value);
    }

    friend BitFlags operator^(BitFlags lhs, BitFlags rhs)
    {
        return BitFlags(lhs.value ^ rhs.value);
    }
};

#endif // LYRA_LIBRARY_COMMON_BITFLAGS_H

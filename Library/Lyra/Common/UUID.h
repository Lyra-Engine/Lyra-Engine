#pragma once

#ifndef LYRA_LIBRARY_COMMON_UUID_H
#define LYRA_LIBRARY_COMMON_UUID_H

#include <Lyra/Common/Hash.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>

namespace lyra
{
    // UUID is 128 bit integer (but we include dashes as well),
    // the string representation also includes the trailing \0
    static constexpr uint UUID_BYTES = 36;
    static constexpr uint UUID_COUNT = UUID_BYTES + 1;

    struct UUID
    {
        uint8_t bytes[UUID_BYTES];
    };

    template <std::size_t... I>
    constexpr UUID make_uuid_impl(const char (&arr)[UUID_COUNT], std::index_sequence<I...>)
    {
        return UUID{{static_cast<std::uint8_t>(arr[I])...}};
    }

    constexpr UUID make_uuid(const char (&arr)[UUID_COUNT])
    {
        return make_uuid_impl(arr, std::make_index_sequence<UUID_BYTES>{});
    }

} // namespace lyra

namespace std
{
    template <>
    struct hash<lyra::UUID>
    {
        std::size_t operator()(const lyra::UUID& uuid) const noexcept
        {
            std::size_t hash = 1469598103934665603ull;
            for (uint32_t i = 0; i < lyra::UUID_BYTES; i++)
                lyra::hash_combine(hash, uuid.bytes[i]);
            return hash;
        }
    };
} // namespace std

#endif // LYRA_LIBRARY_COMMON_UUID_H

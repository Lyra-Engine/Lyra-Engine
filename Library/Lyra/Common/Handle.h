#pragma once

#ifndef LYRA_LIBRARY_COMMON_HANDLE_H
#define LYRA_LIBRARY_COMMON_HANDLE_H

#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>

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

        void reset() { value = T(-1); }

        constexpr static auto type_name() -> CString
        {
            return to_string(TYPE);
        }
    };

    template <typename T>
    struct TypedPointerHandle
    {
        void* handle = nullptr;

        template <typename U>
        U* astype() { return reinterpret_cast<U*>(handle); }

        template <typename U>
        U* astype() const { return reinterpret_cast<U*>(handle); }
    };

    template <typename E, E TYPE, typename T = uint32_t>
    inline bool operator==(const Handle<E, TYPE, T>& lhs, const Handle<E, TYPE, T>& rhs)
    {
        return lhs.value == rhs.value;
    }

    template <typename E, E TYPE, typename T = uint32_t>
    inline bool operator!=(const Handle<E, TYPE, T>& lhs, const Handle<E, TYPE, T>& rhs)
    {
        return lhs.value != rhs.value;
    }

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_HANDLE_H

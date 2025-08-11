#pragma once

#ifndef LYRA_LIBRARY_COMMON_FUNCTION_H
#define LYRA_LIBRARY_COMMON_FUNCTION_H

#include <utility>
#include <cstddef>
#include <functional>

// force inline
#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif

namespace lyra
{

    template <typename F>
    decltype(auto) execute(F&& fn)
    {
        return std::forward<F>(fn)();
    }

    template <typename... Args>
    using Function = std::function<Args...>;

    // primary template for non-function types (default case)
    template <typename T>
    struct function_traits;

    // specialization for free functions
    template <typename R, typename... Args>
    struct function_traits<R (*)(Args...)>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type                  = R;
        template <std::size_t N>
        using arg_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    // specialization for member functions
    template <typename Class, typename R, typename... Args>
    struct function_traits<R (Class::*)(Args...)>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type                  = R;
        using class_type                   = Class;
        template <std::size_t N>
        using arg_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    // specialization for const member functions
    template <typename Class, typename R, typename... Args>
    struct function_traits<R (Class::*)(Args...) const>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type                  = R;
        using class_type                   = Class;
        template <std::size_t N>
        using arg_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    // specialization for functors (objects with operator())
    template <typename T>
    struct function_traits : function_traits<decltype(&T::operator())>
    {
    };

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_FUNCTION_H

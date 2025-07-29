#pragma once

#ifndef LYRA_LIBRARY_COMMON_CONTAINER_H
#define LYRA_LIBRARY_COMMON_CONTAINER_H

#include <map>
#include <array>
#include <deque>
#include <stack>
#include <vector>
#include <optional>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>

namespace lyra
{
    template <typename... T>
    using List = std::forward_list<T...>;

    template <typename... T>
    using Deque = std::deque<T...>;

    template <typename... T>
    using Stack = std::stack<T...>;

    template <typename T, int N>
    using Array = std::array<T, N>;

    template <typename... T>
    using Vector = std::vector<T...>;

    template <typename... T>
    using HashSet = std::unordered_set<T...>;

    template <typename... T>
    using HashMap = std::unordered_map<T...>;

    template <typename... T>
    using TreeMap = std::map<T...>;

    template <typename... T>
    using Optional = std::optional<T...>;

    template <typename... T>
    using InitList = std::initializer_list<T...>;

} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_CONTAINER_H

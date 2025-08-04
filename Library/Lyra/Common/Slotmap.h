#pragma once

#ifndef LYRA_LIBRARY_COMMON_SLOTMAP_H
#define LYRA_LIBRARY_COMMON_SLOTMAP_H

#include <vector>
#include <cstdint>

namespace lyra
{
    // primary template - defaults to false
    template <typename T, typename = void>
    struct has_valid : std::false_type
    {
    };

    // specialization that checks for valid() -> bool
    template <typename T>
    struct has_valid<T, std::void_t<decltype(std::declval<T>().valid())>> : std::bool_constant<std::is_same_v<decltype(std::declval<T>().valid()), bool>>
    {
    };

    template <typename T, typename Deleter, typename I = uint32_t, typename = std::enable_if<has_valid<T>::value, bool>>
    struct Slotmap
    {
        std::vector<T> data = {};
        std::vector<I> free = {};
        I              rear = 0;

        virtual ~Slotmap()
        {
            clear();
        }

        void clear()
        {
            for (auto& item : data)
                if (item.valid())
                    Deleter()(item);
        }

        auto add(const T& value) -> I
        {
            I index = get_next_index();

            data.at(index) = value;
            return index;
        }

        void remove(I index)
        {
            assert(index < data.size());
            Deleter()(data.at(index));
            free.push_back(index);
        }

        auto at(I index) -> T& { return data.at(index); }

        auto at(I index) const -> const T& { return data.at(index); }

    private:
        auto get_next_index() -> I
        {
            // fallback: increase the data array
            if (free.empty()) {
                if (rear < data.size())
                    return rear++;

                resize_data();
                return rear++;
            }

            // find from free list
            I index = free.back();
            free.pop_back();
            return index;
        }

        void resize_data()
        {
            size_t old_size = data.size();
            size_t tgt_size = old_size * 2 + 1;
            size_t new_size = tgt_size > 2048 ? 2048 : tgt_size;
            data.resize(new_size);
            rear = static_cast<I>(old_size);
        }
    };

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_SLOTMAP_H

#ifndef LYRA_LIBRARY_COMMON_SLOTMAP_H
#define LYRA_LIBRARY_COMMON_SLOTMAP_H

#include <vector>
#include <cstdint>

namespace lyra
{
    template <typename T, typename Destroyer, typename I = uint32_t>
    struct Slotmap
    {
        std::vector<T> data = {};
        std::vector<I> free = {};
        size_t         rear = 0;

        auto add(const T& value) -> I
        {
            I index = get_next_index();

            data.at(index) = value;
            return index;
        }

        void remove(I index)
        {
            assert(index < data.size());
            Destroyer()(data.at(index));
            free.push_back(index);
        }

        auto at(I index) -> T& { return data.at(index); }

        auto at(I index) const -> const T& { return data.at(index); }

    private:
        auto get_next_index() -> I
        {
            // fallback: increase the data array
            if (free.empty()) {
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
            size_t size = data.size();
            data.resize(size * 2 + 1);
            rear = size;
        }
    };

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_SLOTMAP_H

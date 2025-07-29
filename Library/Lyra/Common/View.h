#pragma once

#ifndef LYRA_LIBRARY_COMMON_VIEW_H
#define LYRA_LIBRARY_COMMON_VIEW_H

#include <stdexcept>

#include <Lyra/Common/Container.h>

namespace lyra
{
    struct UntypedView
    {
        void*  data = nullptr;
        size_t size = 0;
    };

    template <typename T>
    struct TypedView
    {
        // Iterator type aliases
        using iterator               = T*;
        using const_iterator         = const T*;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        TypedView() : data_(nullptr), count(0ull) {}

        TypedView(T& data) : data_(const_cast<T*>(&data)), count(1) {}
        TypedView(const T&& data) = delete;
        TypedView(T&&)            = delete;

        TypedView(const Vector<T>& data) : data_(const_cast<T*>(data.data())), count(data.size()) {}
        TypedView(const Vector<T>&&) = delete;
        TypedView(Vector<T>&&)       = delete;

        template <size_t N>
        TypedView(T (&data)[N]) : data_(data), count(N) {}

        template <size_t N>
        TypedView(const T (&data)[N]) : data_(const_cast<T*>(data)), count(N) {}

        template <size_t N>
        TypedView(const Array<T, N>& data) : data_(const_cast<T*>(data.data())), count(N) {}

        template <size_t N>
        TypedView(Array<T, N>&&) = delete;

        template <size_t N>
        TypedView(const Array<T, N>&&) = delete;

        TypedView(InitList<T>) = delete;

        size_t size() const { return count; }

        // Forward iterator methods
        iterator begin() noexcept { return data_; }
        iterator end() noexcept { return data_ + count; }

        const_iterator begin() const noexcept { return data_; }
        const_iterator end() const noexcept { return data_ + count; }

        const_iterator cbegin() const noexcept { return data_; }
        const_iterator cend() const noexcept { return data_ + count; }

        // Reverse iterator methods
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

        // additional utility methods for completeness
        bool empty() const noexcept { return count == 0; }

        T*       data() { return data_; }
        const T* data() const { return data_; }

        T&       operator[](size_t index) noexcept { return data_[index]; }
        const T& operator[](size_t index) const noexcept { return data_[index]; }

        T& at(size_t index)
        {
            if (index >= count) {
                throw std::out_of_range("TypedView::at: index out of range");
            }
            return data_[index];
        }

        const T& at(size_t index) const
        {
            if (index >= count) {
                throw std::out_of_range("TypedView::at: index out of range");
            }
            return data_[index];
        }

        T&       front() noexcept { return data_[0]; }
        const T& front() const noexcept { return data_[0]; }

        T&       back() noexcept { return data_[count - 1]; }
        const T& back() const noexcept { return data_[count - 1]; }

    private:
        T*     data_ = nullptr;
        size_t count = 0;
    };

} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_VIEW_H

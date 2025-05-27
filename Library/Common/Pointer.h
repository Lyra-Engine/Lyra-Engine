#ifndef LYRA_LIBRARY_COMMON_POINTER_H
#define LYRA_LIBRARY_COMMON_POINTER_H

#include <memory>

namespace lyra
{
    template <typename T, class Deleter = std::default_delete<T>>
    using Own = std::unique_ptr<T, Deleter>;

    template <typename T>
    using Ref = std::shared_ptr<T>;

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_POINTER_H

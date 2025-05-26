#ifndef LYRA_LIBRARY_COMMON_POINTER_H
#define LYRA_LIBRARY_COMMON_POINTER_H

#include <memory>

namespace lyra
{
    template <class T, class Deleter = std::default_delete<T>>
    using Own = std::unique_ptr<T, Deleter>;

    template <class T>
    using Ref = std::shared_ptr<T>;

} // namespace lyra

#endif // LYRA_LIBRARY_COMMON_POINTER_H

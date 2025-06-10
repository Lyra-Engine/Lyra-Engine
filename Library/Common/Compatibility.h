#ifndef LYRA_LIBRARY_COMMON_COMPATIBILITY_H
#define LYRA_LIBRARY_COMMON_COMPATIBILITY_H

#ifdef USE_PLATFORM_WINDOWS
#include <windows.h>
#undef min          // conflicts with std::min
#undef max          // conflicts with std:max
#undef near         // commonly used words
#undef far          // commonly used words
#undef GENERIC_READ // conflicts with RHI enum
#undef OPAUE
#endif

#endif // LYRA_LIBRARY_COMMON_COMPATIBILITY_H

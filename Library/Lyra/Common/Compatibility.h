#pragma once

#ifndef LYRA_LIBRARY_COMMON_COMPATIBILITY_H
#define LYRA_LIBRARY_COMMON_COMPATIBILITY_H

// NOTE: This header is going to be used by other projects,
// Use something universal (instead of Lyra's internal macro for OS check).
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#undef min          // conflicts with std::min
#undef max          // conflicts with std:max
#undef near         // commonly used words
#undef far          // commonly used words
#undef GENERIC_READ // conflicts with RHI enum
#undef OPAUE
#endif

#endif // LYRA_LIBRARY_COMMON_COMPATIBILITY_H

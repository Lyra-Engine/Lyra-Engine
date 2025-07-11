#ifndef LYRA_TESTLIB_HELPER_GEOMETRY_H
#define LYRA_TESTLIB_HELPER_GEOMETRY_H

#include "./common.h"

struct Geometry
{
    GPUBuffer vbuffer;
    GPUBuffer ibuffer;

    static auto create_triangle() -> Geometry;
    static auto create_overlapping_triangles() -> Geometry;
};

#endif // LYRA_TESTLIB_HELPER_GEOMETRY_H

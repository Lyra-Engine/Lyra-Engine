include(FetchContent)

# define external project
# use a newer commit version to avoid cmake deprecation warning
FetchContent_Declare(
  ktx
  GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
  GIT_TAG        v4.4.0
)

# get properties
FetchContent_GetProperties(ktx)

# populate ktx
if(NOT ktx_POPULATED)
  FetchContent_Populate(ktx)
endif()

# build ktx when needed
set(KTX_FEATURE_DOC            OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_JNI            OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_PY             OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_TESTS          OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_TOOLS          OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_TOOLS_CTS      OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_ETC_UNPACK     OFF CACHE BOOL "" FORCE)
SET(KTX_FEATURE_KTX1           ON  CACHE BOOL "" FORCE)
SET(KTX_FEATURE_KTX2           ON  CACHE BOOL "" FORCE)
SET(KTX_FEATURE_VK_UPLOAD      OFF CACHE BOOL "" FORCE)
SET(KTX_FEATURE_GL_UPLOAD      OFF CACHE BOOL "" FORCE)
SET(KTX_GENERATE_VK_FILES      OFF CACHE BOOL "" FORCE)
SET(KTX_FEATURE_STATIC_LIBRARY ON  CACHE BOOL "" FORCE)
SET(BASISU_SUPPORT_OPENCL      OFF CACHE BOOL "" FORCE)

# KTX-Software has some weird MSVC runtime linker issue.
# It won't build for Debug because it is by default using Release runtime.
# In addition, it compiles too many targets that I don't care.
# Hence, I am going to add a simple library target to replace it.
FetchContent_MakeAvailable(ktx)

# set(KTX_MAIN_SRC
#   ${ktx_SOURCE_DIR}/include/KHR/khr_df.h
#   ${ktx_SOURCE_DIR}/include/ktx.h
#   ${ktx_SOURCE_DIR}/lib/astc_codec.cpp
#   ${ktx_SOURCE_DIR}/lib/basis_sgd.h
#   ${ktx_SOURCE_DIR}/lib/basis_transcode.cpp
#   ${ktx_SOURCE_DIR}/lib/miniz_wrapper.cpp
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_containers.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_containers_impl.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_file_headers.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_transcoder_internal.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_transcoder_uastc.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_transcoder.cpp
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu_transcoder.h
#   ${ktx_SOURCE_DIR}/external/basisu/transcoder/basisu.h
#   ${ktx_SOURCE_DIR}/external/basisu/zstd/zstd.c
#   ${ktx_SOURCE_DIR}/external/etcdec/etcdec.cxx
#   ${ktx_SOURCE_DIR}/lib/checkheader.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/createdfd.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/colourspaces.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/dfd.h
#   ${ktx_SOURCE_DIR}/external/dfdutils/interpretdfd.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/printdfd.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/queries.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/vk2dfd.c
#   ${ktx_SOURCE_DIR}/external/dfdutils/vk2dfd.inl
#   ${ktx_SOURCE_DIR}/external/dfdutils/vulkan/vk_platform.h
#   ${ktx_SOURCE_DIR}/external/dfdutils/vulkan/vulkan_core.h
#   ${ktx_SOURCE_DIR}/lib/etcunpack.cxx
#   ${ktx_SOURCE_DIR}/lib/filestream.c
#   ${ktx_SOURCE_DIR}/lib/filestream.h
#   ${ktx_SOURCE_DIR}/lib/formatsize.h
#   ${ktx_SOURCE_DIR}/lib/gl_format.h
#   ${ktx_SOURCE_DIR}/lib/hashlist.c
#   ${ktx_SOURCE_DIR}/lib/info.c
#   ${ktx_SOURCE_DIR}/lib/ktxint.h
#   ${ktx_SOURCE_DIR}/lib/memstream.c
#   ${ktx_SOURCE_DIR}/lib/memstream.h
#   ${ktx_SOURCE_DIR}/lib/strings.c
#   ${ktx_SOURCE_DIR}/lib/swap.c
#   ${ktx_SOURCE_DIR}/lib/texture.c
#   ${ktx_SOURCE_DIR}/lib/texture.h
#   ${ktx_SOURCE_DIR}/lib/texture2.c
#   ${ktx_SOURCE_DIR}/lib/texture2.h
#   ${ktx_SOURCE_DIR}/lib/texture_funcs.inl
#   ${ktx_SOURCE_DIR}/lib/uthash.h
# )
#
# # ktx library
# add_library(ktx STATIC ${KTX_MAIN_SRC})
# target_compile_definitions(ktx PRIVATE LIBKTX KHRONOS_STATIC)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/lib)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/utils)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/external/)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/external/basisu/)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/external/basisu/zstd)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/external/basisu/transcoder)
# target_include_directories(ktx PRIVATE ${ktx_SOURCE_DIR}/other_include)
# target_include_directories(ktx PUBLIC  ${ktx_SOURCE_DIR}/include)

# mark ktx as found
set(ktx_FOUND TRUE)

# re-export target with namespace
add_library(ktx::ktx ALIAS ktx)

# move ktx under folder
set_target_properties(ktx PROPERTIES FOLDER "Vendors")

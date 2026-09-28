# Common toolchain logic for cross-compiling Windows DLLs from Linux
# using clang-cl + lld-link with MSVC headers/libs from msvc-wine.
#
# Do not use directly — include from an arch-specific toolchain file
# that sets CMAKE_SYSTEM_PROCESSOR and TOOLCHAIN_ARCH first.

set(CMAKE_SYSTEM_NAME Windows)

# --- msvc-wine installation root ---

if(NOT DEFINED MSVC_WINE_ROOT)
    set(MSVC_WINE_ROOT "$ENV{HOME}/.msvc")
endif()

# --- Compilers ---

set(CMAKE_C_COMPILER   clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER       lld-link)
set(CMAKE_AR           llvm-lib)
set(CMAKE_RC_COMPILER  llvm-rc)
set(CMAKE_MT           llvm-mt)

# --- Auto-detect MSVC and Windows SDK versions ---

file(GLOB _msvc_vers LIST_DIRECTORIES true "${MSVC_WINE_ROOT}/VC/Tools/MSVC/*")
if(NOT _msvc_vers)
    message(FATAL_ERROR
        "No MSVC installation found under ${MSVC_WINE_ROOT}/VC/Tools/MSVC/. "
        "Run msvc-wine install first: vsdownload.py --accept-license --dest ~/.msvc")
endif()
list(SORT _msvc_vers COMPARE NATURAL ORDER DESCENDING)
list(GET  _msvc_vers 0 MSVC_DIR)

file(GLOB _sdk_vers LIST_DIRECTORIES true "${MSVC_WINE_ROOT}/kits/10/Include/*")
if(NOT _sdk_vers)
    message(FATAL_ERROR
        "No Windows SDK found under ${MSVC_WINE_ROOT}/kits/10/Include/. "
        "Run msvc-wine install first.")
endif()
list(SORT _sdk_vers COMPARE NATURAL ORDER DESCENDING)
list(GET  _sdk_vers 0 SDK_DIR)

string(REGEX REPLACE ".*/([^/]+)$" "\\1" SDK_VER "${SDK_DIR}")
set(LIB_DIR "${MSVC_WINE_ROOT}/kits/10/Lib/${SDK_VER}")

# --- System include paths (-imsvc so clangd sees them as system headers) ---

string(JOIN " " CLANG_IMSVC_FLAGS
    "-imsvc${MSVC_DIR}/include"
    "-imsvc${SDK_DIR}/ucrt"
    "-imsvc${SDK_DIR}/shared"
    "-imsvc${SDK_DIR}/um"
)

set(CMAKE_C_FLAGS_INIT   "${CLANG_IMSVC_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${CLANG_IMSVC_FLAGS}")

# --- Library search paths ---

string(JOIN " " LINK_FLAGS
    "/LIBPATH:${MSVC_DIR}/lib/${TOOLCHAIN_ARCH}"
    "/LIBPATH:${LIB_DIR}/ucrt/${TOOLCHAIN_ARCH}"
    "/LIBPATH:${LIB_DIR}/um/${TOOLCHAIN_ARCH}"
)

set(CMAKE_EXE_LINKER_FLAGS_INIT    "${LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${LINK_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${LINK_FLAGS}")

# --- Don't search Linux system paths ---

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

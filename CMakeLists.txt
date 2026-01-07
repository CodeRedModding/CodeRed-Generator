# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.20)

# ---------------------
# Project Configuration
# ---------------------

# Define the project
project(CodeRedGenerator
        DESCRIPTION "Unreal Engine 3 SDK Generator"
        VERSION 1.2.0
        HOMEPAGE_URL "https://github.com/CodeRedModding/CodeRed-Generator"
        LANGUAGES CXX C)

configure_file(version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp @ONLY)

# Generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ language standard to C++20, C17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Organize targets into folders in IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Enable all, extra, and pedantic warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
else()
    message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. Warning flags not set.")
endif()

# --------------------
# Engine Configuration
# --------------------

# Auto-detect available engines
file(GLOB ENGINE_DIRS
     LIST_DIRECTORIES TRUE
     "${CMAKE_CURRENT_SOURCE_DIR}/engine/*")

set(AVAILABLE_ENGINES "")
foreach(DIR ${ENGINE_DIRS})
    if(IS_DIRECTORY ${DIR})
        get_filename_component(ENGINE_NAME ${DIR} NAME)
        list(APPEND AVAILABLE_ENGINES ${ENGINE_NAME})
    endif()
endforeach()

if(NOT AVAILABLE_ENGINES)
    message(FATAL_ERROR "No engine directories found in engine/")
endif()

# Set default engine to the "Template" engine if it exists, otherwise the first available engine
if("Template" IN_LIST AVAILABLE_ENGINES)
    set(DEFAULT_ENGINE "Template")
else()
    list(GET AVAILABLE_ENGINES 0 DEFAULT_ENGINE)
endif()

# Option to select the engine
set(ENGINE "${DEFAULT_ENGINE}" CACHE STRING "Select the engine to build with")
set_property(CACHE ENGINE PROPERTY STRINGS ${AVAILABLE_ENGINES})

# Validate selected engine
list(FIND AVAILABLE_ENGINES "${ENGINE}" ENGINE_INDEX)
if(ENGINE_INDEX EQUAL -1)
    string(REPLACE ";" ", " ENGINES_STR "${AVAILABLE_ENGINES}")
    message(FATAL_ERROR "Invalid ENGINE value: ${ENGINE}. Available engines: ${ENGINES_STR}")
endif()

message(STATUS "Using engine: ${ENGINE}")

# -------------------
# Build Configuration
# -------------------

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source files
set(SOURCES
    "src/dllmain.cpp"
    "src/utils.cpp"
    "src/Framework/Member.cpp"
    "src/Framework/Printer.cpp"
    "src/Engine.cpp"
)

# Header files
set(HEADERS
    "include/version.hpp"
    "include/dllmain.hpp"
    "include/utils.hpp"
    "include/Framework/Member.hpp"
    "include/Framework/Printer.hpp"
    "include/Engine.hpp"
)

# Engine files
set(ENGINE_SOURCES
    "engine/${ENGINE}/Configuration.cpp"
    "engine/${ENGINE}/GameDefines.cpp"
    "engine/${ENGINE}/PiecesOfCode.cpp"
)
set(ENGINE_HEADERS
    "engine/${ENGINE}/Configuration.hpp"
    "engine/${ENGINE}/GameDefines.hpp"
    "engine/${ENGINE}/PiecesOfCode.hpp"
)

# Organize source and header files into groups for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/engine"
             PREFIX "Engine Files"
             FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
             PREFIX "Source Files"
             FILES ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
             PREFIX "Header Files"
             FILES ${HEADERS})

# Create shared library (DLL)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Define the selected engine
target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE=${ENGINE})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# --------------------------
# Compiler-Specific Settings
# --------------------------

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Compiler definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        _CONSOLE
        _WINDLL
        _UNICODE
        UNICODE
    )

    # Compiler options
    target_compile_options(${PROJECT_NAME} PRIVATE
        /fp:precise                     # Precise floating-point model
        /Gd                             # __cdecl calling convention
        /GS                             # Buffer security checks
        /Oy-                            # Disable frame pointer omission
        /permissive-                    # Standards conformance
        /sdl                            # SDL (Security Development Lifecycle) checks
        /Zc:wchar_t                     # Treat wchar_t as a native type
        /Zc:inline                      # Enforce inline semantics
        /Zc:forScope                    # Enforce for-scope semantics

        # Enable optimizations
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:RelWithDebInfo>:/GL>
        $<$<CONFIG:MinSizeRel>:/GL>

        # Function-level linking
        $<$<CONFIG:Release>:/Gy>
        $<$<CONFIG:RelWithDebInfo>:/Gy>
        $<$<CONFIG:MinSizeRel>:/Gy>

        # Intrinsic functions
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:RelWithDebInfo>:/Oi>
    )

    # Linker definitions
    target_link_options(${PROJECT_NAME} PRIVATE
        /NXCOMPAT           # Non-executable memory protection
        /SUBSYSTEM:CONSOLE  # Console subsystem
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:/SAFESEH>

        # Incremental linking
        $<$<CONFIG:Release>:/LTCG:incremental>
        $<$<CONFIG:RelWithDebInfo>:/LTCG:incremental>
        $<$<CONFIG:MinSizeRel>:/LTCG:incremental>

        # Optimization settings
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
        $<$<CONFIG:RelWithDebInfo>:/OPT:REF>
        $<$<CONFIG:RelWithDebInfo>:/OPT:ICF>
        $<$<CONFIG:MinSizeRel>:/OPT:REF>
        $<$<CONFIG:MinSizeRel>:/OPT:ICF>
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Compiler options
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fms-extensions                 # Enable MS extensions
        -fstack-protector-strong        # Buffer security checks (equivalent to /GS)
        -fno-omit-frame-pointer         # Disable frame pointer omission (equivalent to /Oy-)

        # Enable optimizations for Release builds
        $<$<CONFIG:Release>:-flto=auto>
        $<$<CONFIG:RelWithDebInfo>:-flto=auto>
        $<$<CONFIG:MinSizeRel>:-flto=auto>

        # Function-level linking (equivalent to /Gy)
        $<$<CONFIG:Release>:-ffunction-sections>
        $<$<CONFIG:Release>:-fdata-sections>
        $<$<CONFIG:RelWithDebInfo>:-ffunction-sections>
        $<$<CONFIG:RelWithDebInfo>:-fdata-sections>
        $<$<CONFIG:MinSizeRel>:-ffunction-sections>
        $<$<CONFIG:MinSizeRel>:-fdata-sections>
    )

    # Linker options
    target_link_options(${PROJECT_NAME} PRIVATE
        # Link-time optimization for Release builds
        $<$<CONFIG:Release>:-flto=auto>
        $<$<CONFIG:RelWithDebInfo>:-flto=auto>
        $<$<CONFIG:MinSizeRel>:-flto=auto>

        # Dead code elimination (equivalent to /OPT:REF and /OPT:ICF)
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        $<$<CONFIG:RelWithDebInfo>:-Wl,--gc-sections>
        $<$<CONFIG:MinSizeRel>:-Wl,--gc-sections>
        -fno-use-linker-plugin
    )

    #if MinGW, disable linker plugin usage
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            -fno-use-linker-plugin
        )
    endif()
else()
    message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. Compiler-specific settings not applied.")
endif()

# --------------------------
# Platform-Specific Settings
# --------------------------

if(WIN32)
    # Link against necessary system libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        psapi
    )
elseif(UNIX)
    # Link against necessary system libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread
    )
else()
    message(WARNING "Unknown platform: ${CMAKE_SYSTEM_NAME}. Platform-specific settings not applied.")
endif()

# --------------------
# Output Configuration
# --------------------

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

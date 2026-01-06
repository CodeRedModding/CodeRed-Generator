# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.20)

# ---------------------
# Project Configuration
# ---------------------

# Define the project
project(CodeRedGenerator
        DESCRIPTION "Unreal Engine 3 SDK Generator"
        VERSION 1.1.7
        LANGUAGES CXX C)

# Generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ language standard to C++20, C17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable all, extra, and pedantic warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
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
set_property(CACHE ENGINE PROPERTY STRINGS ${ENGINE_DIRS})

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

# Source files
set(SOURCES
    src/dllmain.cpp
    src/Framework/Member.cpp
    src/Framework/Printer.cpp
    src/Engine.cpp
    engine/${ENGINE}/Configuration.cpp
    engine/${ENGINE}/GameDefines.cpp
    engine/${ENGINE}/PiecesOfCode.cpp
)

# Header files
set(HEADERS
    include/dllmain.hpp
    include/Framework/Member.hpp
    include/Framework/Printer.hpp
    include/Engine.hpp
    engine/${ENGINE}/Configuration.hpp
    engine/${ENGINE}/GameDefines.hpp
    engine/${ENGINE}/PiecesOfCode.hpp
)

# Create shared library (DLL)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Define the selected engine
target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE=${ENGINE})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Platform-specific settings
if(WIN32)
    # Unicode character set
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        UNICODE
        _UNICODE
        _CONSOLE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    # Add WIN32 definition for 32-bit builds
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
    endif()

    # MSVC-specific settings
    if(MSVC)
        # Warning level 3
        target_compile_options(${PROJECT_NAME} PRIVATE /W3)

        # SDL checks
        target_compile_options(${PROJECT_NAME} PRIVATE /sdl)

        # Conformance mode
        target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)

        # Release optimizations
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/Gy>  # Function-level linking
            $<$<CONFIG:Release>:/Oi>  # Intrinsic functions
        )

        # Linker settings
        target_link_options(${PROJECT_NAME} PRIVATE
            /SUBSYSTEM:CONSOLE
            $<$<CONFIG:Release>:/OPT:REF>   # Optimize references
            $<$<CONFIG:Release>:/OPT:ICF>   # COMDAT folding
        )
    endif()
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

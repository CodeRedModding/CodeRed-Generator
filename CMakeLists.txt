cmake_minimum_required(VERSION 3.21)

project(CodeRedGenerator
    LANGUAGES CXX
    VERSION 1.2.0
)

# Version header, generated into the build tree so the source checkout stays clean.
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Framework/Version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/Version.hpp"
    @ONLY
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ".clangd" reads the compile database from "build/", while every preset builds into a
# subfolder of it. Link the stable path at whichever preset configured last, so clangd
# follows the active one. Visual Studio generators write no database, so they are skipped.
if(CMAKE_GENERATOR MATCHES "Ninja|Makefiles"
   AND CMAKE_BINARY_DIR MATCHES "^${CMAKE_SOURCE_DIR}/build/.")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
        SYMBOLIC
        RESULT CODERED_LINK_RESULT
    )
    if(NOT CODERED_LINK_RESULT EQUAL 0)
        message(WARNING "Could not link build/compile_commands.json: ${CODERED_LINK_RESULT}")
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(CODERED_WERROR "Treat warnings as errors" OFF)

# clang-cl reads "-Wall" as "-Weverything", and only the frontend variant tells the two clang drivers apart.
set(CODERED_MSVC_FRONTEND $<OR:$<CXX_COMPILER_ID:MSVC>,$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>>)

set(SHARED_COMPILE_OPTIONS
    $<${CODERED_MSVC_FRONTEND}:
        /W4
        $<$<BOOL:${CODERED_WERROR}>:/WX>
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang,AppleClang,GNU>,$<NOT:${CODERED_MSVC_FRONTEND}>>:
        -Wall -Wextra -Wpedantic
        $<$<BOOL:${CODERED_WERROR}>:-Werror>
    >
)

set(SHARED_COMPILE_DEFINITIONS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Sources shared by every engine, compiled once per engine because "Engine.hpp" pulls in
# that engine's own headers.
set(CODERED_SOURCES
    dllmain.cpp
    pch.cpp
    Engine/Engine.cpp
    Framework/Member.cpp
    Framework/Printer.cpp
)

# Builds one generator DLL for the engine folder of the same name. Each engine is a separate
# target so that building the project checks all of them, rather than only the selected one.
function(add_engine ENGINE_NAME)
    set(ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/${ENGINE_NAME}")

    if(NOT IS_DIRECTORY "${ENGINE_DIR}")
        message(FATAL_ERROR "No engine folder at ${ENGINE_DIR}")
    endif()

    set(TARGET_NAME "CodeRedGenerator.${ENGINE_NAME}")

    add_library(${TARGET_NAME} SHARED
        ${CODERED_SOURCES}
        "${ENGINE_DIR}/Configuration.cpp"
        "${ENGINE_DIR}/GameDefines.cpp"
        "${ENGINE_DIR}/PiecesOfCode.cpp"
    )

    # Static assertions over the engine's class layout, built where an engine provides them.
    if(EXISTS "${ENGINE_DIR}/LayoutCheck.cpp")
        target_sources(${TARGET_NAME} PRIVATE "${ENGINE_DIR}/LayoutCheck.cpp")
    endif()

    target_include_directories(${TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
    )
    target_compile_definitions(${TARGET_NAME} PRIVATE
        ${SHARED_COMPILE_DEFINITIONS}
        ENGINE=${ENGINE_NAME}
    )
    target_compile_options(${TARGET_NAME} PRIVATE ${SHARED_COMPILE_OPTIONS})
    target_link_libraries(${TARGET_NAME} PRIVATE psapi)
    target_precompile_headers(${TARGET_NAME} PRIVATE pch.hpp)

    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        OUTPUT_NAME "CodeRedGenerator.${ENGINE_NAME}"
        PREFIX ""
    )

endfunction()

# Every folder under "Engine" that holds a Configuration.cpp is an engine.
file(GLOB CODERED_ENGINE_DIRS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*")
foreach(ENGINE_DIR ${CODERED_ENGINE_DIRS})
    if(EXISTS "${ENGINE_DIR}/Configuration.cpp")
        get_filename_component(ENGINE_NAME "${ENGINE_DIR}" NAME)
        add_engine(${ENGINE_NAME})
    endif()
endforeach()

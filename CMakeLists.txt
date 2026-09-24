cmake_minimum_required(VERSION 3.26)

# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------

# NOTE:
# Update executable name in .github/workflows/cmake.yml when changing this.
set(MAIN_PROJECT_NAME "SimpleGameEngine")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(
        ${MAIN_PROJECT_NAME}
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------

option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(USE_ASAN "Use Address Sanitizer" OFF)
option(USE_MSAN "Use Memory Sanitizer" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(
            CMAKE_INSTALL_PREFIX
            "${CMAKE_SOURCE_DIR}/install_dir"
            CACHE PATH "Installation directory"
            FORCE
    )
endif()

set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb_info "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb_info}>")

# ------------------------------------------------------------------------------
# RPATH
# ------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(
            CMAKE_INSTALL_RPATH
            "@loader_path;@loader_path/lib;@loader_path/../lib"
    )

    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

elseif(UNIX)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(
            CMAKE_INSTALL_RPATH
            "$ORIGIN;$ORIGIN/lib;$ORIGIN/../lib"
    )

    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
endif()

# ------------------------------------------------------------------------------
# Project CMake modules
# ------------------------------------------------------------------------------

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

include(FetchContent)

find_package(Threads REQUIRED)

# ------------------------------------------------------------------------------
# SDL2
# ------------------------------------------------------------------------------

find_package(
        SDL2
        CONFIG
        QUIET
        COMPONENTS SDL2
)

if(NOT TARGET SDL2::SDL2)
    message(STATUS "SDL2 not found; fetching SDL2")

    set(SDL_TEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.30.0
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL2)
endif()

if(NOT TARGET SDL2::SDL2)
    message(FATAL_ERROR "SDL2::SDL2 target was not created")
endif()

# SDL2main is optional.
#
# Try to discover it separately when using an installed SDL2 package.
# FetchContent may already have created the target.
if(NOT TARGET SDL2::SDL2main)
    find_package(
            SDL2
            CONFIG
            QUIET
            COMPONENTS SDL2main
    )
endif()

# ------------------------------------------------------------------------------
# SDL2_image
# ------------------------------------------------------------------------------

find_package(SDL2_image CONFIG QUIET)

if(NOT TARGET SDL2_image::SDL2_image)
    message(STATUS "SDL2_image not found; fetching SDL2_image")

    set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL2_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG release-2.8.2
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL2_image)
endif()

if(TARGET SDL2_image::SDL2_image)
    set(SDL2_IMAGE_TARGET SDL2_image::SDL2_image)
elseif(TARGET SDL2_image)
    set(SDL2_IMAGE_TARGET SDL2_image)
else()
    message(FATAL_ERROR "No usable SDL2_image target was found")
endif()

# ------------------------------------------------------------------------------
# SDL2_mixer
# ------------------------------------------------------------------------------

find_package(SDL2_mixer CONFIG QUIET)

if(NOT TARGET SDL2_mixer::SDL2_mixer)
    message(STATUS "SDL2_mixer not found; fetching SDL2_mixer")

    set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)

    set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL2_mixer
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
            GIT_TAG release-2.8.0
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL2_mixer)
endif()

if(TARGET SDL2_mixer::SDL2_mixer)
    set(SDL2_MIXER_TARGET SDL2_mixer::SDL2_mixer)
elseif(TARGET SDL2_mixer)
    set(SDL2_MIXER_TARGET SDL2_mixer)
else()
    message(FATAL_ERROR "No usable SDL2_mixer target was found")
endif()

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

file(
        GLOB_RECURSE SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------

add_executable(
        ${MAIN_EXECUTABLE_NAME}
        ${SOURCES}

        include/engine/utils/windowsRescale.h
        include/testApp/backgroundLayer.h
)

# ------------------------------------------------------------------------------
# Include directories
# ------------------------------------------------------------------------------

target_include_directories(
        ${MAIN_EXECUTABLE_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(
        ${MAIN_EXECUTABLE_NAME}
        SYSTEM PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/generated/include"
)

# ------------------------------------------------------------------------------
# Libraries
# ------------------------------------------------------------------------------

target_link_libraries(
        ${MAIN_EXECUTABLE_NAME}
        PRIVATE
        Threads::Threads
)

# SDL documentation specifies SDL2main, when present, before SDL2.
if(TARGET SDL2::SDL2main)
    target_link_libraries(
            ${MAIN_EXECUTABLE_NAME}
            PRIVATE
            SDL2::SDL2main
    )
endif()

target_link_libraries(
        ${MAIN_EXECUTABLE_NAME}
        PRIVATE
        SDL2::SDL2
        ${SDL2_IMAGE_TARGET}
        ${SDL2_MIXER_TARGET}
)

# ------------------------------------------------------------------------------
# Compiler / sanitizer configuration
# ------------------------------------------------------------------------------

set_compiler_flags(
        RUN_SANITIZERS TRUE
        TARGET_NAMES ${MAIN_EXECUTABLE_NAME}
)

if(MSVC)
    set_target_properties(
            ${MAIN_EXECUTABLE_NAME}
            PROPERTIES
            VS_USER_PROPS "${CMAKE_SOURCE_DIR}/disable_modules.props"
    )

    target_compile_options(
            ${MAIN_EXECUTABLE_NAME}
            PRIVATE
            /experimental:module-
    )

    if(USE_ASAN)
        target_compile_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                "$<${debug_mode}:/fsanitize=address>"
        )
    endif()

else()
    if(USE_ASAN)
        target_compile_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                "$<${debug_mode}:-fsanitize=address,undefined>"
        )

        target_link_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                "$<${debug_mode}:-fsanitize=address,undefined>"
        )

    elseif(USE_MSAN AND UNIX AND NOT APPLE)
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            message(
                    FATAL_ERROR
                    "MemorySanitizer requires Clang"
            )
        endif()

        target_compile_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                "$<${debug_mode}:-fsanitize=memory,undefined>"
                "$<${debug_mode}:-fsanitize-memory-track-origins>"
        )

        target_link_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                "$<${debug_mode}:-fsanitize=memory,undefined>"
                "$<${debug_mode}:-fsanitize-memory-track-origins>"
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# CI-specific warning suppression
# ------------------------------------------------------------------------------

if(DEFINED ENV{GITHUB_ACTIONS})
    if(MSVC)
        target_compile_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                /wd4100
                /wd4101
                /wd4189
        )
    else()
        target_compile_options(
                ${MAIN_EXECUTABLE_NAME}
                PRIVATE
                -Wno-unused-parameter
                -Wno-unused-variable
                -Wno-unused-function
                -Wno-unused-but-set-variable
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# Assets
# ------------------------------------------------------------------------------

add_custom_command(
        TARGET ${MAIN_EXECUTABLE_NAME}
        POST_BUILD

        COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${MAIN_EXECUTABLE_NAME}>/assets"

        COMMENT "Copying assets"
)

# ------------------------------------------------------------------------------
# Runtime DLLs on Windows
# ------------------------------------------------------------------------------

if(WIN32)
    add_custom_command(
            TARGET ${MAIN_EXECUTABLE_NAME}
            POST_BUILD

            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${MAIN_EXECUTABLE_NAME}>
            $<TARGET_FILE_DIR:${MAIN_EXECUTABLE_NAME}>

            COMMAND_EXPAND_LISTS

            COMMENT "Copying runtime DLLs"
    )
endif()

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------

install(
        TARGETS ${MAIN_EXECUTABLE_NAME}
        RUNTIME DESTINATION ${DESTINATION_DIR}
)
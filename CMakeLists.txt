cmake_minimum_required(VERSION 3.26)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
# for now, the project name is used as the executable name
set(MAIN_PROJECT_NAME "SimpleGameEngine")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")


project(${MAIN_PROJECT_NAME})
# set(CMAKE_PROJECT_VERSION_MAJOR 0)
# set(CMAKE_PROJECT_VERSION_MINOR 0)
# set(CMAKE_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# You aren't supposed to be using this, but convenience...
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR} "include/*")
file(GLOB_RECURSE TESTS RELATIVE ${CMAKE_SOURCE_DIR} "test/*.cpp")
include_directories(include)
include_directories(src)

option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(USE_ASAN "Use Address Sanitizer" OFF)
option(USE_MSAN "Use Memory Sanitizer" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "install_dir" CACHE PATH "..." FORCE)
endif()

# disable sanitizers when releasing executables without explicitly requested debug info
# use generator expressions to set flags correctly in both single and multi config generators
set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb}>")

###############################################################################

# custom functions

function(detect_libcpp)
    set(OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(TEST_STDLIB_SRC [=[
    #include <iostream>
    int main() {}
    ]=])
    try_compile(HAS_LIBCPP SOURCE_FROM_CONTENT test_stdlib.cpp ${TEST_STDLIB_SRC})
    set(CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})
    unset(OLD_CMAKE_CXX_FLAGS)
endfunction()


function(set_custom_stdlib_and_sanitizers target add_apple_asan)
    if(MSVC)
        # see https://gitlab.kitware.com/cmake/cmake/-/issues/24922
        set_target_properties(${target} PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/disable_modules.props")
        target_compile_options(${target} PRIVATE /experimental:module-)
        if(USE_ASAN)
            target_compile_options(${target} PRIVATE "$<${debug_mode}:/fsanitize=address>")
        endif()
        return()
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT WIN32)
        detect_libcpp()
        if(HAS_LIBCPP)
            # see also https://stackoverflow.com/a/70382484
            target_compile_options(${target} PRIVATE -stdlib=libc++)
            target_link_options(${target} PRIVATE -stdlib=libc++)
        else()
            # fall back to libstdc++
            target_compile_options(${target} PRIVATE -stdlib=libstdc++)
            target_link_options(${target} PRIVATE -stdlib=libstdc++)
        endif()
    endif()

    if(APPLE)
        # first check Apple since Apple is also a kind of Unix
        if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND add_apple_asan MATCHES true)
            if(USE_ASAN)
                target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
                target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            endif()
        endif()
    elseif(UNIX)
        if(USE_ASAN)
            # check leaks on Linux since macOS does not support the leaks sanitizer yet
            # leaks sanitizer is enabled by default on Linux, so we do not need to enable it explicitly
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
        elseif(USE_MSAN)
            # use semi-colons instead of spaces to separate arguments
            # it is recommended to quote generator expressions in order to avoid unintentional splitting
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins;-Wl,-rpath,tools/llvm-project/build/lib>")
        endif()
    endif()
endfunction()
include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

# --- SDL2: System Search with FetchContent Fallback ---
find_package(SDL2 QUIET)

if(SDL2_FOUND)
    message(STATUS "Using system SDL2 (Version: ${SDL2_VERSION})")
else()
    message(STATUS "SDL2 not found on system. Downloading...")
    include(FetchContent)
    FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.30.0  # Matches your local version
    )
    # We set this to avoid building SDL tests/examples which saves time
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2)
endif()

# =========================================================================
# NEW: SDL2_image and SDL2_mixer configuration
# =========================================================================

# --- SDL2_image: System Search with FetchContent Fallback ---
find_package(SDL2_image QUIET)

if(SDL2_image_FOUND)
    message(STATUS "Using system SDL2_image")
    set(SDL2_IMAGE_LIB SDL2_image::SDL2_image)
else()
    message(STATUS "SDL2_image not found on system. Downloading...")
    include(FetchContent)
    FetchContent_Declare(
            SDL2_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG release-2.8.2
    )
    set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2_image)
    # The FetchContent target is usually named SDL2_image
    set(SDL2_IMAGE_LIB SDL2_image)
endif()

# --- SDL2_mixer: System Search with FetchContent Fallback ---
find_package(SDL2_mixer QUIET)

if(SDL2_mixer_FOUND)
    message(STATUS "Using system SDL2_mixer")
    set(SDL2_MIXER_LIB SDL2_mixer::SDL2_mixer)
else()
    message(STATUS "SDL2_mixer not found on system. Downloading...")
    include(FetchContent)
    FetchContent_Declare(
            SDL2_mixer
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
            GIT_TAG release-2.8.0
    )
    set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)

    # Disable problematic external formats to ensure build succeeds
    set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)          # ADDED: Disable WavPack

    FetchContent_MakeAvailable(SDL2_mixer)
    # The FetchContent target is usually named SDL2_mixer
    set(SDL2_MIXER_LIB SDL2_mixer)
endif()


# external dependencies with FetchContent
include(FetchContent)
#
# set(FETCHCONTENT_QUIET OFF)
# set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
#
# NOTE: Also update env vars used for caching in
#         - .github/actions/configure-cmake/action.yml
#         - .github/workflows/cmake.yml
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
#
FetchContent_MakeAvailable(googletest)

###############################################################################

# external dependencies with find_package

# find_package(Threads REQUIRED)

###############################################################################

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing name here
add_executable(${MAIN_EXECUTABLE_NAME} ${SOURCES}
        include/engine/utils/windowsRescale.h
)

# NOTE: Add all defined targets (e.g. executables, libraries, etc. )
# NOTE: RUN_SANITIZERS is optional, if it's not present it will default to true
# --- Linking the Libraries ---
find_package(Threads REQUIRED)

target_link_libraries(${MAIN_EXECUTABLE_NAME}
        PRIVATE
        SDL2::SDL2main
        SDL2::SDL2              # Works whether system-found or downloaded
        ${SDL2_IMAGE_LIB}
        ${SDL2_MIXER_LIB}
)
add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${MAIN_EXECUTABLE_NAME}>/assets"
)
if(WIN32)
    add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${MAIN_EXECUTABLE_NAME}>
            $<TARGET_FILE_DIR:${MAIN_EXECUTABLE_NAME}>
            COMMAND_EXPAND_LISTS
    )
endif()
set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})
# set_compiler_flags(TARGET_NAMES ${MAIN_EXECUTABLE_NAME} ${FOO} ${BAR})
# where ${FOO} and ${BAR} represent additional executables or libraries
# you want to compile with the set compiler flags

###############################################################################

# use SYSTEM so cppcheck and clang-tidy do not report warnings from these directories
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE generated/include)
# target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ext/<SomeHppLib>/include)
# target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${<SomeLib>_SOURCE_DIR}/include)
# target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${<SomeLib>_BINARY_DIR}/lib)
# target_link_libraries(${MAIN_EXECUTABLE_NAME} <SomeLib>)

###############################################################################

# copy binaries to "bin" folder; these are uploaded as artifacts on each release
# DESTINATION_DIR is set as "bin" in cmake/Options.cmake:6
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

###############################################################################
#enable_testing()
#
#add_executable(
#        test_oop
#        ${HEADERS}
#        ${TESTS}
#        ${SOURCES}
#)
#
#target_link_libraries(
#        test_oop
#        GTest::gtest_main
#)
#
#include(GoogleTest)
#gtest_discover_tests(test_oop)
copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(FILES tastatura.txt config.json DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
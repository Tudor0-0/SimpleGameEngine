cmake_minimum_required(VERSION 3.26)

# NOTE: Update executable name in .github/workflows/cmake.yml:25 when changing it here
set(MAIN_PROJECT_NAME "SimpleGameEngine")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME} CXX)

# --- RPATH Configuration for Linux / macOS ---
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/lib;$ORIGIN/../lib;$ORIGIN/../_deps")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(USE_ASAN "Use Address Sanitizer" OFF)
option(USE_MSAN "Use Memory Sanitizer" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "install_dir" CACHE PATH "..." FORCE)
endif()

set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb}>")

# --- Custom Functions & Includes ---
function(detect_libcpp)
    set(OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(TEST_STDLIB_SRC "#include <iostream>\nint main() {}")
    try_compile(HAS_LIBCPP SOURCE_FROM_CONTENT test_stdlib.cpp ${TEST_STDLIB_SRC})
    set(CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})
    unset(OLD_CMAKE_CXX_FLAGS)
endfunction()

function(set_custom_stdlib_and_sanitizers target add_apple_asan)
    if(MSVC)
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
            target_compile_options(${target} PRIVATE -stdlib=libc++)
            target_link_options(${target} PRIVATE -stdlib=libc++)
        else()
            target_compile_options(${target} PRIVATE -stdlib=libstdc++)
            target_link_options(${target} PRIVATE -stdlib=libstdc++)
        endif()
    endif()

    if(APPLE)
        if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND add_apple_asan)
            if(USE_ASAN)
                target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
                target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            endif()
        endif()
    elseif(UNIX)
        if(USE_ASAN)
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
        elseif(USE_MSAN)
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins;-Wl,-rpath,tools/llvm-project/build/lib>")
        endif()
    endif()
endfunction()

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

# --- Dependencies ---
find_package(Threads REQUIRED)

# SDL2
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    include(FetchContent)
    FetchContent_Declare(SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-2.30.0)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2)
endif()

# SDL2_image
find_package(SDL2_image QUIET)
if(SDL2_image_FOUND)
    set(SDL2_IMAGE_LIB SDL2_image::SDL2_image)
else()
    include(FetchContent)
    FetchContent_Declare(SDL2_image GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git GIT_TAG release-2.8.2)
    set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2_image)
    set(SDL2_IMAGE_LIB SDL2_image)
endif()

# SDL2_mixer
find_package(SDL2_mixer QUIET)
if(SDL2_mixer_FOUND)
    set(SDL2_MIXER_LIB SDL2_mixer::SDL2_mixer)
else()
    include(FetchContent)
    FetchContent_Declare(SDL2_mixer GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git GIT_TAG release-2.8.0)
    set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2_mixer)
    set(SDL2_MIXER_LIB SDL2_mixer)
endif()

# --- Executable Setup ---
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(${MAIN_EXECUTABLE_NAME}
        ${SOURCES}
        include/engine/utils/windowsRescale.h
        include/testApp/backgroundLayer.h
)

target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE
        include
        src
        SYSTEM generated/include
)

target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
        Threads::Threads
        SDL2::SDL2main
        SDL2::SDL2
        ${SDL2_IMAGE_LIB}
        ${SDL2_MIXER_LIB}
)

# --- Post-Build & CI Optimization ---
set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

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

if(DEFINED ENV{GITHUB_ACTIONS})
    if(MSVC)
        target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE /wd4100 /wd4101 /wd4189)
    else()
        target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE
                -Wno-unused-parameter
                -Wno-unused-variable
                -Wno-unused-function
                -Wno-unused-but-set-variable
        )
    endif()
endif()

# --- Installation ---
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
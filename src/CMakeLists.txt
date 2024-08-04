cmake_minimum_required(VERSION 3.22)

# CMake modules
list( APPEND CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_LIST_DIR}/cmake"
)

project(SvenBXT LANGUAGES C CXX ASM)

include(PlatformInfo)
include(InputFilesList)

include_directories(${CMAKE_SOURCE_DIR})

if (COMPILER_MSVC)
    set(WINDOWS_FILES
        iface.cpp
        metahook_emulation.cpp
        svenmod_emulation.cpp)
endif()

set(CLIENT_FILES
    cl_dll/cdll_int.cpp
    cl_dll/cdll_int.h
    cl_dll/hud.cpp
    cl_dll/hud.h
    cl_dll/hud_jumpspeed.cpp
    cl_dll/hud_jumpspeed.h
    cl_dll/hud_origin.cpp
    cl_dll/hud_origin.h
    cl_dll/hud_speedometer.cpp
    cl_dll/hud_speedometer.h
    cl_dll/hud_timer.cpp
    cl_dll/hud_timer.h
    cl_dll/hud_viewangles.cpp
    cl_dll/hud_viewangles.h
    cl_dll/parsemsg.cpp
    cl_dll/parsemsg.h
    cl_dll/view.cpp
    cl_dll/view.h)

set(SERVER_FILES
    dlls/server.cpp
    dlls/server.h)

set(ENGINE_FILES
    engine/gl_screen.cpp
    engine/gl_screen.h)

set(MAIN_FILES
    SvenBXT.cpp
    SvenBXT.h
    Utils.cpp
    Utils.h
    hlsdk_mini.hpp)

source_group("cl_dll" FILES ${CLIENT_FILES})
source_group("dlls" FILES ${SERVER_FILES})
source_group("engine" FILES ${ENGINE_FILES})
source_group("" FILES ${MAIN_FILES} ${WINDOWS_FILES})

add_library(SvenBXT SHARED
    ${WINDOWS_FILES}
    ${CLIENT_FILES}
    ${SERVER_FILES}
    ${ENGINE_FILES}
    ${MAIN_FILES})

add_subdirectory("external")

# Preprocessor definitions
set( COMMON_DEFINES "" )				# Preprocessor definitions for all targets

# Add platform defines to common defines
set( COMMON_DEFINES "${COMMON_DEFINES} ${PLATFORM_DEFINES}" )

if( COMPILER_GNU )

    set( COMMON_DEFINES
            ${COMMON_DEFINES}
            _stricmp=strcasecmp
            _strnicmp=strncasecmp
            _snprintf=snprintf
            _alloca=alloca
            _vsnprintf=vsnprintf
            _snwprintf=swprintf
    )
elseif( COMPILER_MSVC )

    # Disable "unsafe" warnings
    set( COMMON_DEFINES
            ${COMMON_DEFINES}
            _CRT_SECURE_NO_WARNINGS
            _SCL_SECURE_NO_WARNINGS
    )
endif()

if (COMPILER_GNU)
set_target_properties(SvenBXT PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        COMPILE_FLAGS "${COMPILE_FLAGS} -fvisibility=hidden"
        LINKER_FLAGS "${LINKER_FLAGS} -fvisibility=hidden"
)
endif()

target_compile_definitions( SvenBXT PUBLIC
        ${COMMON_DEFINES}
)

# Threads setup
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

# Modules
find_package(Threads REQUIRED)

# Link with dependencies
target_link_libraries(SvenBXT
        Threads::Threads
        funchook-static
)
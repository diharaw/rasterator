cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("SoftRast")

# External Dependencies
set(LIB_SDL2_DEBUG "${PROJECT_SOURCE_DIR}/libs/Debug/SDL2d.lib")
set(LIB_SDL2_RELEASE "${PROJECT_SOURCE_DIR}/libs/Release/SDL2.lib")

set(LIB_SDL2_MAIN_DEBUG "${PROJECT_SOURCE_DIR}/libs/Debug/SDL2maind.lib")
set(LIB_SDL2_MAIN_RELEASE "${PROJECT_SOURCE_DIR}/libs/Release/SDL2main.lib")

set(LIB_SDL2_TTF_DEBUG "${PROJECT_SOURCE_DIR}/libs/Debug/SDL2_ttf.lib")
set(LIB_SDL2_TTF_RELEASE "${PROJECT_SOURCE_DIR}/libs/Release/SDL2_ttf.lib")

set(LIB_FREETYPE_DEBUG "${PROJECT_SOURCE_DIR}/libs/Debug/freetyped.lib")
set(LIB_FREETYPE_RELEASE "${PROJECT_SOURCE_DIR}/libs/Release/freetype.lib")


# Headers
set(SOFTRAST_HEADERS "${PROJECT_SOURCE_DIR}/src/Application.h"
                     "${PROJECT_SOURCE_DIR}/src/Platform.h")

# Sources
set(SOFTRAST_SOURCES "${PROJECT_SOURCE_DIR}/src/Application.cpp")

# Source groups
source_group("Headers" FILES ${SOFTRAST_HEADERS})
source_group("Sources" FILES ${SOFTRAST_SOURCES})

# Include paths
set(SOFTRAST_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/SDL/include"
                          "${PROJECT_SOURCE_DIR}/external/SDL_ttf")

include_directories("${SOFTRAST_INCLUDE_DIRS}")

if(APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

    add_executable(SoftRast MACOSX_BUNDLE ${SOFTRAST_HEADERS} ${SOFTRAST_SOURCES})
    set(MACOSX_BUNDLE_BUNDLE_NAME "com.terminus.softrast")
elseif(WIN32)
    add_executable(SoftRast WIN32 ${SOFTRAST_HEADERS} ${SOFTRAST_SOURCES})
elseif(EMSCRIPTEN)
    message(STATUS "Building for Emscripten...")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(SoftRast ${SOFTRAST_HEADERS} ${SOFTRAST_SOURCES})
    set_target_properties(SoftRast PROPERTIES LINK_FLAGS "-s --std=c++14")
else()
    message(STATUS "Building for default platform...")
    add_executable(SoftRast ${SOFTRAST_HEADERS} ${SOFTRAST_SOURCES})
endif()

target_link_libraries(SoftRast debug "${LIB_SDL2_DEBUG}" optimized "${LIB_SDL2_RELEASE}")
target_link_libraries(SoftRast debug "${LIB_SDL2_MAIN_DEBUG}" optimized "${LIB_SDL2_MAIN_RELEASE}")
target_link_libraries(SoftRast debug "${LIB_SDL2_TTF_DEBUG}" optimized "${LIB_SDL2_TTF_RELEASE}")
target_link_libraries(SoftRast debug "${LIB_FREETYPE_DEBUG}" optimized "${LIB_FREETYPE_RELEASE}")

if(WIN32)
    target_link_libraries(SoftRast "user32.lib"
                                    "gdi32.lib"
                                    "winmm.lib"
                                    "imm32.lib"
                                    "ole32.lib"
                                    "oleaut32.lib"
                                    "version.lib"
                                    "uuid.lib"
                                    "dinput8.lib"
                                    "kernel32.lib"
                                    "winspool.lib"
                                    "shell32.lib"
                                    "comdlg32.lib"
                                    "advapi32.lib")
elseif(APPLE)
    # Link Apple Frameworks
    target_link_libraries(SoftRast "-framework CoreVideo"
                                    "-framework Cocoa"
                                    "-framework CoreAudio"
                                    "-framework AudioUnit"
                                    "-framework AudioToolbox"
                                    "-framework Carbon"
                                    "-framework ForceFeedback"
                                    "-framework IOKit"
                                    "-framework ApplicationServices"
                                    "-liconv"
                                    "-framework QuartzCore")
endif() 
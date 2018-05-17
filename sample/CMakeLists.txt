cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Sources
set(SAMPLE_SOURCES "${PROJECT_SOURCE_DIR}/sample/main.cpp")

# Source groups
source_group("Sources" FILES ${SAMPLE_SOURCES})

if(APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

    add_executable(sample MACOSX_BUNDLE ${SAMPLE_SOURCES})
    set(MACOSX_BUNDLE_BUNDLE_NAME "com.dihara.rasterator")
elseif(WIN32)
    add_executable(sample ${SAMPLE_SOURCES})
elseif(EMSCRIPTEN)
    message(STATUS "Building for Emscripten...")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(sample ${SAMPLE_SOURCES})
    set_target_properties(sample PROPERTIES LINK_FLAGS "-s --std=c++14")
else()
    message(STATUS "Building for default platform...")
    add_executable(sample ${SAMPLE_SOURCES})
endif()

target_link_libraries(sample Rasterator)
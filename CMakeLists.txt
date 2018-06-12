cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("Rasterator")

IF(APPLE)
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
ENDIF()

# Include paths
set(RASTERATOR_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/external/SDL/include"
                            "${PROJECT_SOURCE_DIR}/external/SDL/include"
                            "${CMAKE_CURRENT_BINARY_DIR}/external/assimp/include"
                            "${PROJECT_SOURCE_DIR}/external/assimp/include"
                            "${PROJECT_SOURCE_DIR}/external/stb"
                            "${PROJECT_SOURCE_DIR}/include")

include_directories("${RASTERATOR_INCLUDE_DIRS}")

if (NOT EMSCRIPTEN)
    add_subdirectory(external/SDL)
endif()
                
add_subdirectory(external/assimp)

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/sample")
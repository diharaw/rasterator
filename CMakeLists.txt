cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("Rasterator")

# Include paths
set(RASTERATOR_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/SDL/include"
                            "${PROJECT_SOURCE_DIR}/external/SDL_ttf"
                            "${PROJECT_SOURCE_DIR}/external/assimp/include/assimp"
                            "${PROJECT_SOURCE_DIR}/include")

include_directories("${RASTERATOR_INCLUDE_DIRS}")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/sample")
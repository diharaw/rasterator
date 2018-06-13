cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FindOpenMP)

if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Headers
set(RASTERATOR_HEADERS "${PROJECT_SOURCE_DIR}/include/application.hpp"
                       "${PROJECT_SOURCE_DIR}/include/rasterator.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/mat3.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/mat4.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/quat.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_float4.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_float8.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_mat4x4.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_mat4x8.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_vec4x4.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/simd_vec4x8.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/transform.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/utility.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/vec2.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/vec3.hpp"
                       "${PROJECT_SOURCE_DIR}/include/math/vec4.hpp")

# Sources
set(RASTERATOR_SOURCES "${PROJECT_SOURCE_DIR}/src/application.cpp"
                       "${PROJECT_SOURCE_DIR}/src/rasterator.cpp")

# Source groups
source_group("Headers" FILES ${RASTERATOR_HEADERS})
source_group("Sources" FILES ${RASTERATOR_SOURCES})

add_library(Rasterator ${RASTERATOR_HEADERS} ${RASTERATOR_SOURCES})

if (NOT EMSCRIPTEN)
	target_link_libraries(Rasterator SDL2-static)
    target_link_libraries(Rasterator SDL2main)
else()    
    set_target_properties(Rasterator PROPERTIES LINK_FLAGS "-O3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2")
endif()

target_link_libraries(Rasterator assimp)

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

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
endif()

target_link_libraries(Rasterator assimp)

if(EMSCRIPTEN)
    set_target_properties(Rasterator PROPERTIES CMAKE_CXX_FLAGS "-s USE_SDL=2")
endif()

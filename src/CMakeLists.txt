cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# External Dependencies
set(LIB_ASSIMP_RELEASE "${PROJECT_SOURCE_DIR}/libs/assimp.lib")
set(LIB_SDL2_RELEASE "${PROJECT_SOURCE_DIR}/libs/SDL2.lib")
set(LIB_SDL2_MAIN_RELEASE "${PROJECT_SOURCE_DIR}/libs/SDL2main.lib")
set(LIB_SDL2_TTF_RELEASE "${PROJECT_SOURCE_DIR}/libs/SDL2_ttf.lib")
set(LIB_FREETYPE_RELEASE "${PROJECT_SOURCE_DIR}/libs/freetype.lib")


# Headers
set(RASTERATOR_HEADERS "${PROJECT_SOURCE_DIR}/include/application.hpp"
                       "${PROJECT_SOURCE_DIR}/include/geometry.hpp"
                       "${PROJECT_SOURCE_DIR}/include/model.hpp"
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
                       "${PROJECT_SOURCE_DIR}/src/geometry.cpp"
                       "${PROJECT_SOURCE_DIR}/src/model.cpp"
                       "${PROJECT_SOURCE_DIR}/src/rasterator.cpp")

# Source groups
source_group("Headers" FILES ${RASTERATOR_HEADERS})
source_group("Sources" FILES ${RASTERATOR_SOURCES})

add_library(Rasterator ${RASTERATOR_HEADERS} ${RASTERATOR_SOURCES})

target_link_libraries(Rasterator "${LIB_ASSIMP_RELEASE}")
target_link_libraries(Rasterator "${LIB_SDL2_RELEASE}")
target_link_libraries(Rasterator "${LIB_SDL2_MAIN_RELEASE}")
target_link_libraries(Rasterator "${LIB_SDL2_TTF_RELEASE}")
target_link_libraries(Rasterator "${LIB_FREETYPE_RELEASE}")

if(WIN32)
    target_link_libraries(Rasterator "user32.lib"
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
    target_link_libraries(Rasterator "-framework CoreVideo"
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
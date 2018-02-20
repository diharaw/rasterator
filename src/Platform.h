
#ifdef __EMSCRIPTEN__
    #define TE_PLATFORM_EMSCRIPTEN
#elif _WIN64
    #define TE_PLATFORM_WIN64
    #define TE_PLATFORM_WIN32
#elif _WIN32
    #define TE_PLATFORM_WIN32
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
        #define TE_PLATFORM_IPHONE_SIMULATOR
    #elif TARGET_OS_IPHONE
        #define TE_PLATFORM_IPHONE
    #else
        #define TARGET_OS_OSX 1
        #define TE_PLATFORM_MACOS
    #endif
#elif __ANDROID__
    #define TE_PLATFORM_ANDROID
#elif __linux
    #define TE_PLATFORM_LINUX
#elif __unix
    #define TE_PLATFORM_UNIX
#elif __posix
    #define TE_PLATFORM_POSIX
#endif

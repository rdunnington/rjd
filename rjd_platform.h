#pragma once

#define RJD_PLATFORM 1

// Platforms
#ifdef _WIN32
	#define RJD_PLATFORM_WINDOWS 1
#endif

#if __APPLE__ && __MACH__
	#define RJD_PLATFORM_OSX 1
#endif

// Compilers
#ifdef _MSC_VER
	#define RJD_COMPILER_MSVC 1
#elif __clang__
	#define RJD_COMPILER_CLANG 1
#elif __GNUC__
	#define RJD_COMPILER_GCC 1
#else
	#error Unknown platform.
#endif

#if RJD_COMPILER_MSVC
	#define RJD_INLINE __forceinline
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_INLINE static inline __attribute__(always_inline)
#endif


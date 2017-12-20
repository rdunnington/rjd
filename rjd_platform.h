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
	#define RJD_FORCEINLINE __forceinline
	#define RJD_FORCEALIGN(type, alignment) __declspec(align(alignment)) type
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_FORCEINLINE static inline __attribute__(always_inline)
	#define RJD_FORCEALIGN(type, alignment) type __attribute__((aligned(alignment)))
#else
	#error Unhandled compiler
#endif

#define RJD_ISALIGNED(p, align) (((uintptr_t)(p) & ((align)-1)) == 0)

#if RJD_COMPILER_MSVC
	#define RJD_COMPILER_MSVC_ONLY(code) code
	#define RJD_COMPILER_GCC_ONLY(code)
	#define RJD_COMPILER_CLANG_ONLY(code)
#elif RJD_COMPILER_GCC
	#define RJD_COMPILER_MSVC_ONLY(code)
	#define RJD_COMPILER_GCC_ONLY(code) code
	#define RJD_COMPILER_CLANG_ONLY(code)
#elif RJD_COMPILER_CLANG
	#define RJD_COMPILER_MSVC_ONLY(code)
	#define RJD_COMPILER_GCC_ONLY(code)
	#define RJD_COMPILER_CLANG_ONLY(code) code
#else
	#define RJD_COMPILER_MSVC_ONLY(code)
	#define RJD_COMPILER_GCC_ONLY(code)
	#define RJD_COMPILER_CLANG_ONLY(code)
#endif

#if RJD_ENABLE_SHORTNAMES
	#define FORCEINLINE RJD_FORCEINLINE
	#define FORCEALIGN RJD_FORCEALIGN
	#define ISALIGNED RJD_ISALIGNED
#endif


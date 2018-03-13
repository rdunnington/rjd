#pragma once

#define RJD_PLATFORM 1

// Platforms
#if _WIN32 || __CYGWIN__
	#define RJD_PLATFORM_WINDOWS 1
#elif __APPLE__ && __MACH__
	#define RJD_PLATFORM_OSX 1
#else
	#error Unknown platform.
#endif

// Compilers
#ifdef _MSC_VER
	#define RJD_COMPILER_MSVC 1
#elif __clang__
	#define RJD_COMPILER_CLANG 1
#elif __GNUC__
	#define RJD_COMPILER_GCC 1
#else
	#error Unknown compiler.
#endif

#if RJD_COMPILER_MSVC
	#define RJD_FORCE_INLINE __forceinline
	#define RJD_FORCE_ALIGN(type, alignment) __declspec(align(alignment)) type
	#define restrict __restrict
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_FORCE_INLINE static inline __attribute__((always_inline))
	#define RJD_FORCE_ALIGN(type, alignment) type __attribute__((aligned(alignment)))
#else
	#error Unhandled compiler
#endif

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
	#define FORCE_INLINE RJD_FORCE_INLINE
	#define FORCE_ALIGN RJD_FORCE_ALIGN
#endif


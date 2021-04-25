#pragma once

#define RJD_PLATFORM_H 1

// Platforms
#if _WIN32 || __CYGWIN__
	#define RJD_PLATFORM_WINDOWS 1
	#define RJD_PLATFORM_OSX 0
#elif __APPLE__ && __MACH__
	#define RJD_PLATFORM_OSX 1
	#define RJD_PLATFORM_WINDOWS 0
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
	#define RJD_FORCE_ALIGN(alignment, type) __declspec(align(alignment)) type
	#define restrict __restrict
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_FORCE_INLINE static inline __attribute__((always_inline))
	#define RJD_FORCE_ALIGN(alignment, type) type __attribute__((aligned(alignment)))
#else
	#error Unhandled compiler
#endif

#if RJD_COMPILER_MSVC
	#if defined(_M_X86) || defined(_M_X64)
		#define RJD_ARCH_64 1
	#elif defined(_M_IX86)
		#define RJD_ARCH_32 1
	#else
		#error Unknown architecture
	#endif
#elif RJD_COMPILER_GCC
	#if defined(__x86_64) || defined(__x86_64__)
		#define RJD_ARCH_64 1
	#elif defined(i386) || defined(__i386) || defined(__i386__)
		#define RJD_ARCH_32 1
	#else
		#error Unknown architecture
	#endif
#elif RJD_COMPILER_CLANG
    #if defined(__x86_64) || defined(__x86_64__)
        #define RJD_ARCH_64 1
    #elif defined(i386) || defined(__i386) || defined(__i386__)
        #define RJD_ARCH_32 1
    #else
        #error Unknown architecture
    #endif
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

#if RJD_COMPILER_MSVC
	#pragma warning(disable:4201) // nonstandard extension used: nameless struct/union (gcc and clang support this)
	#pragma warning(disable:4221) // nonstandard extension used: initializing struct with address of local variable (gcc and clang support this)

#elif RJD_COMPILER_CLANG
	#pragma clang diagnostic ignored "-Wmissing-braces" // clang is paranoid about zero-init for nested structs
#endif

#if defined(__OBJC__)
	#define RJD_LANG_OBJC 1
#else
	#define RJD_LANG_OBJC 0
#endif

#if defined(__cplusplus)
	#define RJD_LANG_CPP 1
#else
	#define RJD_LANG_CPP 0
#endif

#if RJD_IMPL 
#if RJD_PLATFORM_WINDOWS
	#if RJD_COMPILER_MSVC
		#pragma warning(push)
		#pragma warning(disable:5105) // windows.h triggers warning C5105: macro expansion producing 'defined' has undefined behavior
	#endif

	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEAN
	#define NOMINMAX
	#include <windows.h>
	#include <combaseapi.h>

	#if RJD_COMPILER_MSVC
		#pragma warning(pop)
	#endif
#endif // RJD_PLATFORM_WINDOWS
#endif // RJD_IMPL


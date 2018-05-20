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
	#error TODO: Implement for clang
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

	#define PLATFORM_WINDOWS	RJD_PLATFORM_WINDOWS
	#define PLATFORM_OSX		RJD_PLATFORM_OSX

	#define ARCH_64				RJD_ARCH_64
	#define ARCH_32				RJD_ARCH_32

	#define COMPILER_MSVC		RJD_COMPILER_MSVC
	#define COMPILER_GCC        RJD_COMPILER_GCC
	#define COMPILER_CLANG      RJD_COMPILER_CLANG

	#define COMPILER_MSVC_ONLY	RJD_COMPILER_MSVC_ONLY
	#define COMPILER_GCC_ONLY	RJD_COMPILER_GCC_ONLY
	#define COMPILER_CLANG_ONLY	RJD_COMPILER_CLANG_ONLY
#endif

#if RJD_COMPILER_MSVC
	#pragma warning(disable:4204) // nonstandard extension used: non-constant aggregate initializer (this is ok in C99)
	#pragma warning(disable:4201) // nonstandard extension used: nameless struct/union (all major compilers support this)
#endif


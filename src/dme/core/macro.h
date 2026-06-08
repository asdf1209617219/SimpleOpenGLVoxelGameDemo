#pragma once
#ifndef DME_MACRO_H

#define DME_MACRO_H
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string>
#include <type_traits>

/*
The following preprocessor identifiers specify compiler, OS, and architecture.
All definitions have a value of 1 or 0, use '#if' instead of '#ifdef'.
*/

/**
Compiler defines, see http://sourceforge.net/p/predef/wiki/Compilers/
*/
#if defined(_MSC_VER)
	#if _MSC_VER >= 1950
	#define DME_VC 17
	#elif _MSC_VER >= 1920
	#define DME_VC 16
	#elif _MSC_VER >= 1910
	#define DME_VC 15
	#elif _MSC_VER >= 1900
	#define DME_VC 14
	#elif _MSC_VER >= 1800
	#define DME_VC 12
	#elif _MSC_VER >= 1700
	#define DME_VC 11
	#elif _MSC_VER >= 1600
	#define DME_VC 10
	#elif _MSC_VER >= 1500
	#define DME_VC 9
	#else
	#error "Unknown VC version"
	#endif
#elif defined(__clang__)
	#define DME_CLANG 1
	#if defined (__clang_major__) 
	#define DME_CLANG_MAJOR __clang_major__
	#elif defined (_clang_major)
	#define DME_CLANG_MAJOR _clang_major
	#else
	#define DME_CLANG_MAJOR 0
	#endif	
#elif defined(__GNUC__) // note: __clang__ implies __GNUC__
	#define DME_GCC 1
#else
	#error "Unknown compiler"
#endif

/**
Operating system defines, see http://sourceforge.net/p/predef/wiki/OperatingSystems/
*/
#if defined(_WIN64)
#define DME_WIN64 1
#elif defined(_WIN32) // note: _M_PPC implies _WIN32
#define DME_WIN32 1
#elif defined(__linux__) || defined (__EMSCRIPTEN__)
#define DME_LINUX 1
#elif defined(__APPLE__)
#define DME_OSX 1
#elif defined(__NX__)
#define DME_SWITCH 1
#else
#error "Unknown operating system"
#endif

/**
Architecture defines, see http://sourceforge.net/p/predef/wiki/Architectures/
*/
#if defined(__x86_64__) || defined(_M_X64)
#define DME_X64 1
#elif defined(__i386__) || defined(_M_IX86) || defined (__EMSCRIPTEN__)
#define DME_X86 1
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define DME_A64 1
#elif defined(__arm__) || defined(_M_ARM)
#define DME_ARM 1
#elif defined(__ppc__) || defined(_M_PPC) || defined(__CELLOS_LV2__)
#define DME_PPC 1
#else
#error "Unknown architecture"
#endif

/**
SIMD defines
*/
#if !defined(DME_SIMD_DISABLED)
#if defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64) || (defined (__EMSCRIPTEN__) && defined(__SSE2__))
#define DME_SSE2 1
#endif
#if defined(_M_ARM) || defined(__ARM_NEON__) || defined(__ARM_NEON)
#define DME_NEON 1
#endif
#if defined(_M_PPC) || defined(__CELLOS_LV2__)
#define DME_VMX 1
#endif
#endif

/**
define anything not defined on this platform to 0
*/
#ifndef DME_VC
#define DME_VC 0
#endif
#ifndef DME_CLANG
#define DME_CLANG 0
#endif
#ifndef DME_GCC
#define DME_GCC 0
#endif
#ifndef DME_WIN64
#define DME_WIN64 0
#endif
#ifndef DME_WIN32
#define DME_WIN32 0
#endif
#ifndef DME_LINUX
#define DME_LINUX 0
#endif
#ifndef DME_OSX
#define DME_OSX 0
#endif
#ifndef DME_SWITCH
#define DME_SWITCH 0
#endif
#ifndef DME_X64
#define DME_X64 0
#endif
#ifndef DME_X86
#define DME_X86 0
#endif
#ifndef DME_A64
#define DME_A64 0
#endif
#ifndef DME_ARM
#define DME_ARM 0
#endif
#ifndef DME_PPC
#define DME_PPC 0
#endif
#ifndef DME_SSE2
#define DME_SSE2 0
#endif
#ifndef DME_NEON
#define DME_NEON 0
#endif
#ifndef DME_VMX
#define DME_VMX 0
#endif

/**
family shortcuts
*/
// compiler
#define DME_GCC_FAMILY (DME_CLANG || DME_GCC)
// os
#define DME_WINDOWS_FAMILY (DME_WIN32 || DME_WIN64)
#define DME_LINUX_FAMILY DME_LINUX
#define DME_APPLE_FAMILY DME_OSX                              // equivalent to #if __APPLE__
#define DME_UNIX_FAMILY (DME_LINUX_FAMILY || DME_APPLE_FAMILY) // shortcut for unix/posix platforms
#if defined(__EMSCRIPTEN__)
#define DME_EMSCRIPTEN 1
#else
#define DME_EMSCRIPTEN 0
#endif
// architecture
#define DME_INTEL_FAMILY (DME_X64 || DME_X86)
#define DME_ARM_FAMILY (DME_ARM || DME_A64)
#define DME_P64_FAMILY (DME_X64 || DME_A64) // shortcut for 64-bit architectures

/**
Force inline macro
*/
#if DME_VC
#define DME_INLINE __forceinline
#elif DME_LINUX // Workaround; Fedora Core 3 do not agree with force inline and PxcPool
#define DME_INLINE inline
#elif DME_GCC_FAMILY
#define DME_INLINE inline __attribute__((always_inline))
#else
#define DME_INLINE inline
#endif

/**
[[no_unique_address]]
*/
#if DME_VC
#define DME_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define DME_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

/**
Alignment macros

DME_ALIGN_PREFIX and DME_ALIGN_SUFFIX can be used for type alignment instead of aligning individual variables as follows:
DME_ALIGN_PREFIX(16)
struct A {
...
} DME_ALIGN_SUFFIX(16);
This declaration style is parsed correctly by Visual Assist.
*/
#ifndef DME_ALIGN
	#if DME_WINDOWS_FAMILY
		#define DME_ALIGN(alignment, decl) __declspec(align(alignment)) decl
		#define DME_ALIGN_PREFIX(alignment) __declspec(align(alignment))
		#define DME_ALIGN_SUFFIX(alignment)
	#elif DME_GCC_FAMILY
		#define DME_ALIGN(alignment, decl) decl __attribute__((aligned(alignment)))
		#define DME_ALIGN_PREFIX(alignment)
		#define DME_ALIGN_SUFFIX(alignment) __attribute__((aligned(alignment)))
	#elif defined __CUDACC__
		#define DME_ALIGN(alignment, decl) __align__(alignment) decl
		#define DME_ALIGN_PREFIX(alignment)
		#define DME_ALIGN_SUFFIX(alignment) __align__(alignment))
	#else
		#define DME_ALIGN(alignment, decl)
		#define DME_ALIGN_PREFIX(alignment)
		#define DME_ALIGN_SUFFIX(alignment)
	#endif
#endif

//assert
//#define DME_ASSERT(A) assert(A)

#if DME_VC
#include <crtdbg.h>
/*
namespace dme::core {
	inline void _assert_alert(const char* mesg) {
		_RPTF0(_CRT_ASSERT, mesg);
	}
	inline void _assert_alert(const std::string& mesg) {
		_RPTF0(_CRT_ASSERT, mesg.c_str());
	}
}
*/
#endif

inline void _DME_debug_assert_breakpoint() {
	int _temp_breakpoint = 1;
}

#ifdef DME_DEBUG
	#if DME_VC
		//debug模式下进行检查，第一个条件不成立时输出报错信息 reference _STL_VERIFY
		#define DME_ASSERT(cond, mesg)			\
		if (!(cond)) {							\
			_RPTF0(_CRT_ASSERT, mesg);			\
			_DME_debug_assert_breakpoint();		\
		}
	#elif DME_LINUX
		#define DME_ASSERT(cond, mesg) assert(cond)
	#elif DME_GCC_FAMILY
		#define DME_ASSERT(cond, mesg) assert(cond)
	#else
		#define DME_ASSERT(cond, mesg) assert(cond)
	#endif
#else
	#define DME_ASSERT(cond, mesg)
#endif

#if DME_VC
#include <crtdbg.h>
//非debug模式也会检查，第一个条件不成立时输出报错信息 reference _STL_VERIFY
#define DME_CHECK(cond, mesg)			\
if (!(cond)) {							\
	_RPTF0(_CRT_ASSERT, mesg);	\
}
#elif DME_LINUX
#define DME_ASSERT(cond, mesg) assert(cond)
#elif DME_GCC_FAMILY
#define DME_ASSERT(cond, mesg) assert(cond)
#else
#define DME_ASSERT(cond, mesg) assert(cond)
#endif

//warning
#if DME_GCC_FAMILY
#define COMPILE_WARNING(msg) _Pragma("GCC warning \"" msg "\"")
#elif DME_VC
#define COMPILE_WARNING(msg) __pragma(message("WARNING: " msg))
#else
#define COMPILE_WARNING(msg) // 其他编译器忽略
#endif

//与std::malloc相同，但会在分配失败时输出错误信息
#define DME_MALLOC(size) [](size_t s) { \
	void* data = std::malloc(s); \
	DME_CHECK(data, "std::malloc failed to allocate " + std::to_string(s) + " bytes" ); \
	return data; \
}((size))

//与std::free相同
#define DME_FREE(ptr) std::free((ptr))


#define _DME_EVAL(...)  _DME_EVAL1(_DME_EVAL1(_DME_EVAL1(__VA_ARGS__)))
#define _DME_EVAL1(...) _DME_EVAL2(_DME_EVAL2(_DME_EVAL2(__VA_ARGS__)))
#define _DME_EVAL2(...) _DME_EVAL3(_DME_EVAL3(_DME_EVAL3(__VA_ARGS__)))
#define _DME_EVAL3(...) _DME_EVAL4(_DME_EVAL4(_DME_EVAL4(__VA_ARGS__)))
#define _DME_EVAL4(...) _DME_EVAL5(_DME_EVAL5(_DME_EVAL5(__VA_ARGS__)))
#define _DME_EVAL5(...) __VA_ARGS__

#define _DME_EMPTY_()
#define _DME_DEFER_(...) __VA_ARGS__ _DME_EMPTY_()
#define _DME_EVAL_ARGS_NEXT(macro, x, ...)  macro(x) __VA_OPT__(, _DME_DEFER_(_DME_EVAL_ARGS_NEXT_I)()(macro, __VA_ARGS__))
#define _DME_EVAL_ARGS_NEXT_I() _DME_EVAL_ARGS_NEXT

#define _DME_FOR_EACH_EVAL(macro, x, ...)  macro(x) __VA_OPT__( _DME_DEFER_(_DME_FOR_EACH_EVAL_I)()(macro, __VA_ARGS__))
#define _DME_FOR_EACH_EVAL_I() _DME_FOR_EACH_EVAL

//宏循环展开，第一个参数传入处理单个参数的宏函数，会对不定参数的每个参数进行处理，不会保留参数之间的逗号。目前递归栈深度为243
#define DME_EVAL_FOREACH(macro, ...) __VA_OPT__( _DME_EVAL(_DME_DEFER_(_DME_FOR_EACH_EVAL_I)()(macro, __VA_ARGS__)) )

//对不定参数的每个参数进行处理，会保留参数之间的逗号。
#define DME_EVAL_ARGS(macro, ...) __VA_OPT__( _DME_EVAL(_DME_DEFER_(_DME_EVAL_ARGS_NEXT_I)()(macro, __VA_ARGS__)) )

//使用实例：static inline auto& Var1 = DME_SVAR_REF(Class1());
#define DME_SVAR_REF(var) *[] { \
	static auto _var_ = var; \
	return &_var_; \
}()


#define DME_GAME_TEST

//#define DME_CONTAINER_CONSTEXPR
//#define DME_CONTAINER_CONSTEXPR constexpr

//资源文件夹路径
#define DME_PATH_ASSETS "assets/"
//block纹理路径
#define DME_PATH_TEX_BLOCK DME_PATH_ASSETS "texture/block/"
//item纹理路径
#define DME_PATH_TEX_ITEM DME_PATH_ASSETS "texture/item/"
//skybox路径
#define DME_PATH_TEX_SKYBOX DME_PATH_ASSETS "texture/skybox/"
//ui纹理路径
#define DME_PATH_TEX_UI DME_PATH_ASSETS "texture/ui/"

//block shader路径
#define DME_PATH_SHADER_BLOCK DME_PATH_ASSETS "shader/opengl/block/"
//ui shader路径
#define DME_PATH_SHADER_UI DME_PATH_ASSETS "shader/opengl/ui/"

#endif

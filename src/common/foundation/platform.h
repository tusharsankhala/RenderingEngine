#pragma once

#include <stdint.h>

#if !defined(_MSC_VER)
#include <signal.h>
#endif

// MACROS /////////////////////////////////////////////////

#define ArraySize(array)		( sizeof(array)/sizeof((array)[0]) )

#if defined (_MSC_VER)
#define ENGINE_INLINE								inline
#define ENGINE_FINLINE								__forceinline
#define ENGINE_DEBUG_BREAK							__debugbreak();
#define ENGINE_DISABLE_WARNING(warning_number)		__pragma( warning( disable : warning_number ) )
#define ENGINE_CONCAT_OPERATOR(x, y)				x##y
#else
#define ENGINE_INLINE								inline
#define ENGINE_FINLINE								always_inline
#define ENGINE_DEBUG_BREAK							raise(SIGTRAP);
#define ENGINE_CONCAT_OPERATOR(x, y)				x##y
#endif // MSVC

#define ENGINE_STRINGIZE( L )						#L
#define ENGINE_MAKESTRING( L )						ENGINE_STRINGIZE( L )
#define ENGINE_CONCAT(x, y)							ENGINE_CONCAT_OPERATOR( x, y )
#define ENGINE_LINE_STRING							ENGINE_MAKESTRING( __LINE__ )
#define ENGINE_FILELINE(MESSAGE)					__FILE__ "(" ENGINE_LINE_STRING ") : " MESSAGE

// Unique Names
#define ENGINE_UNIQUE_SUFFIX(PARAM)					RAPTOR_CONCAT(PARAM, __LINE__ )

// Native types typedefs /////////////////////////////////////////////////
typedef uint8_t										u8;
typedef uint16_t									u16;
typedef uint32_t									u32;
typedef uint64_t									u64;

typedef int8_t										i8;
typedef int16_t										i16;
typedef int32_t										i32;
typedef int64_t										i64;

typedef float										f32;
typedef double										f64;

typedef size_t										sizet;

typedef const char*									cstring;

static const u64									u64_max = UINT64_MAX;
static const i64									i64_max = INT64_MAX;
static const u32									u32_max = UINT32_MAX;
static const i32									i32_max = INT32_MAX;
static const u16									u16_max = UINT16_MAX;
static const i16									i16_max = INT16_MAX;
static const u8										 u8_max = UINT8_MAX;
static const i8										 i8_max = INT8_MAX;
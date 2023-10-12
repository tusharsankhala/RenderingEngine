#include "foundation/bit.h"

#if defined(_MSC_VER)
#include <immintrin.h>
#include <intrin0.h>
#endif

#include <string.h>

namespace Engine
{

    u32 trailing_zeros_u32(u32 x)
    {
#if defined(_MSC_VER)
        return _tzcnt_u32(x);
#else
        return __builtin_ctz(x);
#endif
    }

    u32 leading_zeros_u32(u32 x)
    {
#if defined(_MSC_VER)
        return __lzcnt(x);
#else
        return __builtin_clz(x);
#endif
    }
}
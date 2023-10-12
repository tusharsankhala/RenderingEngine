#pragma once

#include "foundation/platform.h"

namespace Engine
{
    struct Allocator;

    // Common methods ////////////////////////////////////////////////////
    u32                     leading_zeros_u32( u32 x );
    
    u32                     trailing_zeros_u32( u32 x );
    
    // class BitMask //////////////////////////////////////////////////////

    // An abstraction over a bitmask. It provides an easy way to iterate through the
    // indexes of the set bits of a bitmask.  When Shift=0 (platforms with SSE),
    // this is a true bitmask.  On non-SSE, platforms the arithematic used to
    // emulate the SSE behavior works in bytes (Shift=3) and leaves each bytes as
    // either 0x00 or 0x80.
    //
    // For example:
    //   for (int i : BitMask<uint32_t, 16>(0x5)) -> yields 0, 2
    //   for (int i : BitMask<uint64_t, 8, 3>(0x0000000080800000)) -> yields 2, 3
    template < class T, int SignificantBits, int Shift = 0>
    class BitMask
    {
        public:
        // These are useful for unit test.
        using value_type = int;
        using iterator = BitMask;
        using const_iterator = BitMask;

        explicit BitMask( T mask ) : mask_( mask )
        {}

        BitMask& operator++()
        {
            mask_ &= ( mask_ - 1 );
            return *this;
        }

        explicit operator bool() const
        {
            return mask_ != 0;
        }

        int operator*() const
        {
            return LowestBitSet();
        }

        uint32_t LowestBitSet() const
        {
            return trailing_zeros_u32( mask_ ) >> Shift;
        }

        BitMask begin() const 
        {
            return *this;
        }

        BitMask end() const
        {
            return BitMask( 0 );
        }

        uint32_t TrailingZeros() const
        {
            return trailing_zeros_u32( mask_ ); // >> Shift;
        }

        uint32_t LeadingZeros() const
        {
            return leading_zeros_u32( mask_ ); // >> Shift;
        }

    private:
        friend bool operator==( const BitMask& a, const BitMask& b )
        {
            return a.mask_ == b.mask_;
        }

        friend bool operator!=(const BitMask& a, const BitMask& b)
        {
            return a.mask_ != b.mask_;
        }

        T mask_;
    };

}
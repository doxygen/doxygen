/* My tiny gzip decompressor without using zlib. - Joel Yliluoma
 * http://iki.fi/bisqwit/ , http://youtube.com/user/Bisqwit
 * Inspired and influenced by a 13th IOCCC winner program by Ron McFarland */
/* Further optimized based on ideas from tinf library by Joergen Ibsen */
/** @file gunzip.hh @brief TinyDeflate */

/* Fun fact: Contains zero new/delete, and no STL data structures */
/* Distributed under the terms of the Zlib license:

  Copyright (C) 2018 Joel Yliluoma

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include <assert.h>
#include <utility>     // std::forward
#include <cstdint>     // integer sizes
#include <type_traits>
#include <iterator>

#if !1 //Documentation purposes only; the actual prototypes are littered with std::enable_ifs.
/// Deflate(): This is the public method declared (later) in this file.
/// Decompresses (inflates) deflate-compressed data, with a gzip or deflate header.
/// User-supplied functors:
/// @param  input() returns the next byte from the (compressed) input.
/// @param  output(byte) outputs one uncompressed byte.
/// @param  outputcopy(length, offset) copies length uncompressed bytes from offset,
///       Offset is always >= 1.
///       offset 1 means previous byte,
///       offset 2 means previous before that and so on.
///       Note that (offset < length) is not an error and in fact happens frequently.
///       If length=0, offset indicates the largest look-behind window length that
///       you need to be prepared for. The length is a power-of-two in range 256..32768.
//
/// If you want to implement range checking in input, return a negative value
/// from input() when there is no more input.
//
/// If you want to implement range checking in output, add a return value
/// in output(): false=ok, true=abort; and a return value in outputcopy():
/// 0=ok, nonzero=one or more bytes were not writable.
//
/// @returns:
///       0 = decompression complete
///      -1 = data error
///      -2 = input functor returned a value outside 0x00..0xFF range
///      -3 = output functor returned nonzero / bool true value
///      -4 = outputcopy functor returned nonzero remaining length value
//
template<typename InputFunctor, typename OutputFunctor, typename WindowFunctor>
int Deflate(InputFunctor&& input, OutputFunctor&& output, WindowFunctor&& outputcopy);

/// Check README.md for the full list of versions of Deflate() available.

#endif

struct DeflateTrackTagBase{};
struct DeflateTrackNoSize: public DeflateTrackTagBase{};
struct DeflateTrackInSize: public DeflateTrackTagBase{};
struct DeflateTrackOutSize: public DeflateTrackTagBase{};
struct DeflateTrackBothSize: public DeflateTrackTagBase{};


/// The rest of the file is just for the curious about implementation.
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace gunzip_ns
{
    //#define DO_DEFDB_DUMPING

    // If you want more performance at the expense of RAM use,
    // Turn one or more of these settings to false:
    static constexpr bool USE_BITARRAY_TEMPORARY_IN_HUFFMAN_CREATION = false; /* 8 bytes save */
    static constexpr bool USE_BITARRAY_FOR_LENGTHS = false;                   /* 160 bytes save */
    static constexpr bool USE_BITARRAY_FOR_HUFFNODES = false;                 /* 392 bytes save */

    static constexpr unsigned MAX_WINDOW_SIZE = 32768u;

    static_assert(MAX_WINDOW_SIZE >= 1,      "Max window size should be >= 1");
    static_assert(MAX_WINDOW_SIZE <= 32768u, "Window sizes larger than 32768 are not supported by deflate standard. Edit the source code to remove this assert if you need it.");

    //
    #define DEFLATE_USE_DATA_TABLES

    #if !defined(DEFLATE_ALLOCATION_AUTOMATIC) && !defined(DEFLATE_ALLOCATION_STATIC) && !defined(DEFLATE_ALLOCATION_DYNAMIC)
    // Choose one:
    #define DEFLATE_ALLOCATION_AUTOMATIC
    //#define DEFLATE_ALLOCATION_STATIC
    //#define DEFLATE_ALLOCATION_DYNAMIC
    #endif

    constexpr unsigned Flag_InputAbortable  = 0x01;
    constexpr unsigned Flag_OutputAbortable = 0x02;
    constexpr unsigned Flag_TrackIn  = 0x40;
    constexpr unsigned Flag_TrackOut = 0x80;
    constexpr unsigned Flag_NoTrackFlagMask = 0x03;
}

#ifdef DEFLATE_ALLOCATION_DYNAMIC
# include <memory>
#endif

// RandomAccessBitArray: An engine for arrays of data items that are not byte-aligned
template<typename U = std::uint_least64_t>
struct RandomAccessBitArrayBase
{
private:
    static constexpr unsigned Ubytes = sizeof(U), Ubits = Ubytes*8;

    static std::uint_fast64_t Get_Unclean(unsigned Size, const U* data, unsigned index) throw()
    {
        unsigned bitpos = index*Size, unitpos = bitpos / Ubits, shift = bitpos % Ubits;
        std::uint_fast64_t result = data[unitpos] >> shift;
        //assert(Size <= sizeof(result)*8);
        unsigned acquired = Ubits - shift;
        for(; acquired < Size; acquired += Ubits)
        {
            result += (std::uint_fast64_t)data[++unitpos] << acquired;
        }
        return result;
    }
public:
    template<unsigned Size>
    static std::uint_fast64_t Get(const U* data, unsigned index) throw()
    {
        std::uint_fast64_t result = Get_Unclean(Size,data,index);
        return (Size >= sizeof(result)*8) ? result : (result & ((std::uint64_t(1) << Size)-1));
    }

    template<unsigned Size, bool update = false>
    static void Set(U* data, unsigned index, std::uint_fast64_t value) throw()
    {
        unsigned bitpos = index*Size, unitpos = bitpos / Ubits, eat = 0;
        // Make sure our storage unit is at least as bit as value
        //assert(Ubits >= sizeof(value)*8);
        //assert(Size >= sizeof(value)*8 || value < (std::uint64_t(1) << Size));

        if(Size % Ubits != 0)
        {
            unsigned shift = bitpos % Ubits;
            eat = Ubits - shift; if(eat > Size) eat = Size;

            //assert(eat < sizeof(std::uint_fast64_t)*8);
            //assert(shift + eat <= Ubits);
            std::uint_fast64_t vmask = (std::uint64_t(1) << eat)-1;
            if(update)
                data[unitpos] = (data[unitpos] & ~(vmask << shift)) | (value << shift);
            else
                data[unitpos] |= value << shift;
            //assert(eat < sizeof(value)*8);
            value >>= eat;
            ++unitpos;
        }
        if(eat < Size)
            for(unsigned remain = Size-eat; ; ++unitpos)
            {
                eat = Ubits;
                if(eat > remain)
                {
                    eat = remain;
                    if(update)
                    {
                        std::uint_fast64_t vmask = ((std::uint64_t(1) << eat)-1);
                        data[unitpos] = (data[unitpos] & ~vmask) | value;
                    }
                    else
                    {
                        data[unitpos] |= value;
                    }
                    break;
                }
                else
                {
                    data[unitpos] = value;
                    value >>= Ubits/2; value >>= Ubits/2;
                    remain -= Ubits;
                    if(!remain) break;
                }
            }
    }
};

template<unsigned Nbits, typename U = std::uint_least64_t>
struct RandomAccessBitArray
{
    static constexpr unsigned Ubytes = sizeof(U), Ubits = Ubytes*8, Nunits = (Nbits+Ubits-1)/Ubits;
    U data[Nunits];

    template<unsigned Size>
    inline std::uint_fast64_t Get(unsigned index) const throw()
    {
        return RandomAccessBitArrayBase<U>::template Get<Size>(data, index);
    }

    template<unsigned Size, bool update = false>
    inline void Set(unsigned index, std::uint_fast64_t value) throw()
    {
        RandomAccessBitArrayBase<U>::template Set<Size,update>(data, index, value);
    }
};

namespace gunzip_ns
{
    struct dummy{};

    /// Utility: ceil(log2(n))
    template<unsigned long N> struct CeilLog2_s{ static constexpr unsigned result = 1+CeilLog2_s<(N+1)/2>::result; };
    template<> struct CeilLog2_s<0> { static constexpr unsigned result = 0; };
    template<> struct CeilLog2_s<1> { static constexpr unsigned result = 0; };
    template<unsigned long N> static constexpr unsigned CeilLog2 = CeilLog2_s<N>::result;

    /// Utility: floor(log2(n))
    template<unsigned long N> struct FloorLog2_s{ static constexpr unsigned result = 1+FloorLog2_s<N/2>::result; };
    template<> struct FloorLog2_s<0> { static constexpr unsigned result = 0; };
    template<> struct FloorLog2_s<1> { static constexpr unsigned result = 0; };
    template<unsigned long N> static constexpr unsigned FloorLog2 = FloorLog2_s<N>::result;

    /// Utility: smallest unsigned integer type that can store n-bit value
    template<unsigned bits>
    using SmallestType = std::conditional_t< (bits<=16),
                         std::conditional_t< (bits<= 8), std::uint_least8_t,  std::uint_least16_t>,
                         std::conditional_t< (bits<=32), std::uint_least32_t, std::uint_least64_t>>;

    /// testcases
    static_assert(FloorLog2<1> == 0, "FloorLog2 fail"); static_assert(CeilLog2<1> == 0, "CeilLog2 fail");
    static_assert(FloorLog2<2> == 1, "FloorLog2 fail"); static_assert(CeilLog2<2> == 1, "CeilLog2 fail");
    static_assert(FloorLog2<3> == 1, "FloorLog2 fail"); static_assert(CeilLog2<3> == 2, "CeilLog2 fail");
    static_assert(FloorLog2<4> == 2, "FloorLog2 fail"); static_assert(CeilLog2<4> == 2, "CeilLog2 fail");
    static_assert(FloorLog2<5> == 2, "FloorLog2 fail"); static_assert(CeilLog2<5> == 3, "CeilLog2 fail");
    static_assert(FloorLog2<6> == 2, "FloorLog2 fail"); static_assert(CeilLog2<6> == 3, "CeilLog2 fail");
    static_assert(FloorLog2<7> == 2, "FloorLog2 fail"); static_assert(CeilLog2<7> == 3, "CeilLog2 fail");
    static_assert(FloorLog2<8> == 3, "FloorLog2 fail"); static_assert(CeilLog2<8> == 3, "CeilLog2 fail");
    static_assert(FloorLog2<9> == 3, "FloorLog2 fail"); static_assert(CeilLog2<9> == 4, "CeilLog2 fail");

    template<bool packed, unsigned Dimension, unsigned ElementSize>
    struct RandomAccessArray {};

    template<unsigned Dim, unsigned Elem>
    struct RandomAccessArray<true, Dim, Elem>
    {
        RandomAccessBitArray<Dim*Elem> impl;
        inline std::uint_fast64_t Get(unsigned index) const { return impl.template Get<Elem>(index); }
        inline void Set(unsigned index, std::uint_fast32_t value)  { impl.template Set<Elem,true>(index, value); }
        inline void QSet(unsigned index, std::uint_fast32_t value) { impl.template Set<Elem,false>(index, value); }
        template<unsigned Bits>
        inline void WSet(unsigned index, std::uint_fast64_t value) { impl.template Set<Bits,true>(index, value); }
    };

    template<unsigned Dim, unsigned Elem>
    struct RandomAccessArray<false, Dim, Elem>
    {
        typedef SmallestType<Elem> E;
        E data[Dim];
        inline E Get(unsigned index) const        { return data[index]; }
        inline void Set(unsigned index, E value)  { data[index] = value; }
        inline void QSet(unsigned index, E value) { data[index] = value; }
        template<unsigned Bits>
        inline void WSet(unsigned index, std::uint_fast64_t value)
        {
            index *= Bits/Elem;
            for(unsigned b=0; b<Bits; b+=Elem, value>>=Elem)
                QSet(index++, (value % (1u << Elem)));
        }
    };
}


namespace gunzip_ns
{
    //#define DEFL_DO_HUFF_STATS
    template<bool Abortable, unsigned A,unsigned B, typename LengthFunctor>
    bool CreateHuffmanTree(const char*
                           #ifdef DEFL_DO_HUFF_STATS
                                       why
                           #endif
                           , RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES,A,B>& tree,
                           unsigned num_values,
                           LengthFunctor&& ReadLength) throw()
    {
        /* Lengths[] needs to be scanned exactly twice, in forward order.
         * Technically we could use a functor instead of a table,
         * but this would require that the dynamic tree generator
         * can read sections of the compressed data twice,
         * which we currently do not support.
         */

        constexpr unsigned ElemBits = CeilLog2<A-15>; // ceil(log2(A-15)) where A-15 is max value of num_values
        static_assert((1u << B) >= (A-15), "B is too small");
        assert(num_values <= (A-15));

        RandomAccessArray<USE_BITARRAY_TEMPORARY_IN_HUFFMAN_CREATION, 15, ElemBits> offs{}; // 24 or 16 bytes.
        // Theoretically 15.32 bytes for 288 num_values, 9.375 for 32 num_values, 7.97 for 19 num_values.

        // Clear code length count table
        tree.template WSet<(15*B + 63) & ~63>(0, 0); // First 15 needed, but round to nice unit
        // Scan symbol length, and sum code length counts
        #ifdef DEFL_DO_HUFF_STATS
        unsigned largest_treetable_value = 0, largest_offs = 0, smallest_treetable_value = ~0u;
        unsigned largest_treetrans_index=0, largest_treetrans_value=0;
        unsigned longest_length = 0;
        #endif
        for(unsigned a = 0; a < num_values; ++a)
        {
            int length = ReadLength(a); // Note: Can be zero.
            if(Abortable && length < 0) return true;
            if(length)
            {
                unsigned v = tree.Get(0 + length-1)+1;
            #ifdef DEFL_DO_HUFF_STATS
                largest_treetable_value = std::max(largest_treetable_value, v);
                longest_length          = std::max(longest_length, unsigned(length));
            #endif
                //fprintf(stderr, " [%d]%3d CLL (val: %d)\n", length, v, v);
                tree.Set(0 + length-1, v);
            }
            else
            {
                //fprintf(stderr, " [_]%3d CLL (val: 0)\n", 0);
            }
        }

        // Compute offset table for distribution sort
        for(unsigned sum=0, a = 1; a < 16; ++a)
        {
            offs.QSet(a-1, sum);      // starting offset for values that have length "a"
            sum += tree.Get(0 + a-1); // number of values that have length "a"
        }
        #ifdef DEFL_DO_HUFF_STATS
        for(unsigned a=1; a<=longest_length; ++a)
            smallest_treetable_value = std::min(smallest_treetable_value, (unsigned)tree.Get(0 + a-1));
        #endif

        // Create code->symbol translation table (symbols sorted by code)
        for(unsigned value = 0; value < num_values; ++value)
        {
            int length = ReadLength(value); // Note: Can be zero.
            if(Abortable && length < 0) return true;
            if(length)
            {
                unsigned q = offs.Get(length-1); offs.Set(length-1, q+1); // q = offset[length]++;
        #ifdef DEFL_DO_HUFF_STATS
                largest_offs = std::max(q, largest_offs);
                largest_treetrans_index = std::max(largest_treetrans_index, q);
                largest_treetrans_value = std::max(largest_treetrans_value, value);
        #endif
                assert(q < num_values /*&& value < num_values*/);
                //fprintf(stderr, " [x]%3d CLL %d\n", 15+q, value);
                tree.Set(15 + q, value);
            }
        }
        #ifdef DEFL_DO_HUFF_STATS
        std::fprintf(stderr, "Largest \"%12s\"(treetable_value=%4u..%4u, offs=%4u, treetrans_index=%4u, treetrans_value=%4u)\n",
            why, smallest_treetable_value,largest_treetable_value,
            largest_offs, largest_treetrans_index, largest_treetrans_value);
        #endif

        // Largest values observed in the wild:

        // Dyn Lengths: Max treetable_value =255, max offs =285, max treetrans_index =285, max treetrans_value =285
        // Stat Lengths:Max treetable_value =152, max offs =287, max treetrans_index =287, max treetrans_value =287

        // Len Lengths: Max treetable_value = 13, max offs = 18, max treetrans_index = 18, max treetrans_value = 18
        // Dyn Dists:   Max treetable_value = 19, max offs = 29, max treetrans_index = 29, max treetrans_value = 29
        // Stat Dists:  Max treetable_value = 32, max offs = 31, max treetrans_index = 31, max treetrans_value = 31
        return false;
    }

#ifdef DEFLATE_USE_DATA_TABLES
    template<bool=0> // Using a dummy template parameter makes this function and its data weak,
    inline const std::uint_least8_t* GetBTable() throw() // removing linker problems in multi-module use
    {
        static const std::uint_least8_t data[] {
            // Length bases (0-31)
            0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224,255, 0,0,0,
            // Length bits and distance bits (29-60) (overlap 3 bytes)
            //               0x00,0x01,0x01,0x02,0x02,0x13,0x13,0x14,0x14,0x25,0x25,0x26,0x26,
            //0x37,0x37,0x38,0x38,0x49,0x49,0x4A,0x4A,0x5B,0x5B,0x5C,0x5C,0x0D,0x0D,0x00,0x00
            // Reverse-order table
            3*3,17*3,15*3,13*3,11*3,9*3,7*3,5*3,4*3,6*3,8*3,10*3,12*3,14*3,16*3,18*3,0*3,1*3,2*3
        };
        return data;
    }
    //template<bool=0>
    //inline const std::uint_least16_t* GetWTable() throw()
    //{
    //    static const std::uint_least16_t data[32] {
    //         1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577, 0,0 };
    //    return data;
    //}

    //inline unsigned dbase(unsigned distcode) { return GetWTable<>()[distcode]; }
    inline unsigned lbase(unsigned lencode)  { return GetBTable<>()[lencode-257+0] + 3; }
    //inline unsigned dbits(unsigned distcode) { return GetBTable<>()[distcode+29] & 0xF; }
    //inline unsigned lbits(unsigned lencode) { return GetBTable<>()[lencode-257+29] >> 4; }
    inline unsigned rshift(unsigned a)       { return GetBTable<>()[a + 32]; }
#else
    inline unsigned lbase(unsigned lencode)  { return (lencode > 285 ? 3 : ((lencode >= 265) ? (((lencode-257)%4+4) << ((lencode-257)/4-1)) + (lencode==285 ? 2 : 3) : (lencode-254))); }
    inline unsigned rshift(unsigned a)       { if(!a) return 3*3; else if(a>=16) return (a-16)*3; int r = 12 + 7*(a<8) - a*2; return (r<0 ? -r : r)*3; }
#endif
    inline unsigned dbase(unsigned distcode) { return (1 + (distcode>=4 ? ((2+distcode%2) << (distcode/2-1)) : distcode)); }
    inline unsigned dbits(unsigned distcode) { return distcode>=4 ? distcode/2-1 : 0; }
    inline unsigned lbits(unsigned lencode)  { return ((lencode>=265 && lencode<285) ? ((lencode-265)/4+1) : 0); }
    //inline unsigned order(unsigned index)    { return index<3 ? (index+16) : ((index%2) ? (1-index/2)&7 : (6+index/2)); }

    //                            Cortex-M0+      Cortex-M4       x86_64
    // dbase with table           12+64 = 76      12+64 = 76      14+64 = 78
    // dbase with func                    24              22              26
    // lbase with table           12+32 = 48      12+32 = 48      21+64 = 76
    // lbase with func                    54              56              64
    // dbits+lbits with table  12+16+29 = 57   12+16+29 = 57   17+21+29 = 67
    // dbits+lbits with func      14+18 = 32      14+18 = 32      13+20 = 33

    // Support for pre-c++20
    template<typename T>
    using remove_cvref_t = std::remove_reference_t<std::remove_cv_t<T>>;
    // Support for pre-c++20 (result_of is removed in c++20, invoke_result is added in c++17, so neither can be used exclusively)
    template <class T>
    struct result_of { // explain usage
        static_assert((T)false, "result_of<CallableType> is invalid; use "
            "result_of<CallableType(zero or more argument types)> instead.");
    };
    #if __cplusplus > 202000UL
    template <typename F, typename... Args>
    struct result_of<F(Args...)> : std::invoke_result<F, Args...> {};
    #else
    template <typename F, typename... Args>
    struct result_of<F(Args...)> : std::result_of<F(Args...)> {};
    #endif
    template <class T>
    using result_of_t = typename result_of<T>::type;

    #define BEGIN_COND(name) \
        template<typename T, typename=void> struct name : public std::false_type {}; \
        template<typename T> struct name<T, std::enable_if_t<
    #define END_COND(name) \
        , void>> : public std::true_type {}; \
        template<typename T> \
        inline constexpr bool name ## _v = name<T>::value; \

    // Input parameter condition testers:
    BEGIN_COND(is_input_functor)
        std::is_convertible_v<result_of_t<remove_cvref_t<T>()>,int>
    END_COND(is_input_functor)
    BEGIN_COND(is_input_iterator)
        std::is_convertible_v<typename std::iterator_traits<remove_cvref_t<T>>::value_type, unsigned char>
     && std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::input_iterator_tag>
    END_COND(is_input_iterator)
    BEGIN_COND(is_bidir_input)
        std::is_convertible_v<typename std::iterator_traits<remove_cvref_t<T>>::value_type, unsigned char>
     && (std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::forward_iterator_tag>
      || std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::bidirectional_iterator_tag>
      || std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::random_access_iterator_tag>)
    END_COND(is_bidir_input)
    BEGIN_COND(is_size_type)
        std::is_convertible_v<remove_cvref_t<T>, std::size_t> && !std::is_pointer_v<remove_cvref_t<T>>
    END_COND(is_size_type)
    // Output parameter condition testers:
    BEGIN_COND(is_random_iterator)
        std::is_convertible_v<typename std::iterator_traits<remove_cvref_t<T>>::value_type, unsigned char>
     && !std::is_const_v<typename std::iterator_traits<remove_cvref_t<T>>::reference>
     && std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::random_access_iterator_tag>
    END_COND(is_random_iterator)
    BEGIN_COND(is_output_iterator)
        std::is_convertible_v<typename std::iterator_traits<remove_cvref_t<T>>::value_type, unsigned char>
    && !std::is_const_v<typename std::iterator_traits<remove_cvref_t<T>>::reference>
                                && !std::is_pointer_v<remove_cvref_t<T>>
    && (std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::output_iterator_tag>
     || std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::forward_iterator_tag>
     || std::is_same_v<typename std::iterator_traits<remove_cvref_t<T>>::iterator_category, std::bidirectional_iterator_tag>)
    END_COND(is_output_iterator)
    // Output functor & window functor: Returns void or something that can be converted to bool
    BEGIN_COND(is_output_functor)
        std::is_same_v<result_of_t<remove_cvref_t<T>(int)>,void> || std::is_convertible_v<result_of_t<remove_cvref_t<T>(int)>,bool>
    END_COND(is_output_functor)
    BEGIN_COND(is_window_functor)
        std::is_same_v<result_of_t<remove_cvref_t<T>(int,int)>,void> || std::is_convertible_v<result_of_t<remove_cvref_t<T>(int,int)>,int>
    END_COND(is_window_functor)

    BEGIN_COND(is_abortable_input_type)
        !(std::is_same_v<T, unsigned char> || std::is_same_v<T, signed char> || std::is_same_v<T, char>)
    END_COND(is_abortable_input_type)

    #undef END_COND
    #undef BEGIN_COND

    template<typename T>
    constexpr bool DeflAbortable_InFun  = is_abortable_input_type_v<remove_cvref_t<result_of_t<T()>>>;
    template<typename T>
    constexpr bool DeflAbortable_OutFun = std::is_same_v<result_of_t<T(int)>, bool>;
    template<typename T>
    constexpr bool DeflAbortable_WinFun = std::is_convertible_v<remove_cvref_t<result_of_t<T(int,int)>>, int>;

    template<bool Abortable>
    struct OutputHelper
    {
        template<typename OutputFunctor>
        static inline bool output(OutputFunctor&& output, unsigned char byte)
        {
            output(byte);
            return false;
        }
        template<typename WindowFunctor>
        static inline bool outputcopy(WindowFunctor&& outputcopy, std::uint_least16_t length, std::uint_fast32_t offset)
        {
            outputcopy(length, offset);
            return false;
        }
    };

    template<>
    struct OutputHelper<true>
    {
        template<typename OutputFunctor>
        static inline bool output(OutputFunctor&& output, unsigned char byte)
        {
            return output(byte);
        }
        template<typename WindowFunctor>
        static inline bool outputcopy(WindowFunctor&& outputcopy, std::uint_least16_t& length, std::uint_fast32_t offset)
        {
            length = outputcopy(length, offset);
            return length != 0;
        }
    };

    struct SizeTracker_NoOutput
    {
        inline void OutByte()                    { }
        inline void OutBytes(std::uint_fast64_t) { }

        // Dummy forwarders. Do the same as std::forward.
        template<typename T>
        static inline constexpr T&& ForwardOutput(std::remove_reference_t<T>& fun) { return static_cast<T&&>(fun); }
        template<typename T>
        static inline constexpr T&& ForwardOutput(std::remove_reference_t<T>&& fun) { return static_cast<T&&>(fun); }

        template<typename T>
        static inline constexpr T&& ForwardWindow(std::remove_reference_t<T>& fun) { return static_cast<T&&>(fun); }
        template<typename T>
        static inline constexpr T&& ForwardWindow(std::remove_reference_t<T>&& fun) { return static_cast<T&&>(fun); }
    };
    struct SizeTracker_NoInput
    {
        inline void InByte()                    { }
        inline void InBytes(std::uint_fast64_t) { }

        template<typename T>
        static inline constexpr T&& ForwardInput(std::remove_reference_t<T>& fun) { return static_cast<T&&>(fun); }
        template<typename T>
        static inline constexpr T&& ForwardInput(std::remove_reference_t<T>&& fun) { return static_cast<T&&>(fun); }
    };
    struct SizeTracker_DoInput
    {
        std::uint_fast64_t insize=0;

        inline void InByte()                      { ++insize; }
        inline void InBytes(std::uint_fast64_t n) { insize += n; }

        template<typename InputFunctor, std::enable_if_t<!DeflAbortable_InFun<InputFunctor>,gunzip_ns::dummy>...>
        auto ForwardInput(const InputFunctor& input)
        {
            return [&]() { InByte(); return input(); };
        }

        template<typename InputFunctor, std::enable_if_t<DeflAbortable_InFun<InputFunctor>,gunzip_ns::dummy>...>
        auto ForwardInput(const InputFunctor& input)
        {
            return [&]() { auto r = input(); if(!(r & ~0xFF)) { InByte(); } return r; };
        }
    };
    struct SizeTracker_DoOutput
    {
        std::uint_fast64_t outsize=0;

        inline void OutByte()                      { ++outsize; }
        inline void OutBytes(std::uint_fast64_t n) { outsize += n; }

        template<typename OutputFunctor, std::enable_if_t<!DeflAbortable_OutFun<OutputFunctor>,gunzip_ns::dummy>...>
        auto ForwardOutput(const OutputFunctor& output)
        {
            return [&](unsigned char byte) { OutByte(); return output(byte); };
        }

        template<typename OutputFunctor, std::enable_if_t<DeflAbortable_OutFun<OutputFunctor>,gunzip_ns::dummy>...>
        auto ForwardOutput(const OutputFunctor& output)
        {
            return [&](unsigned char byte) { auto r = output(byte); if(!r) { OutByte(); } return r; };
        }

        template<typename WindowFunctor, std::enable_if_t<!DeflAbortable_WinFun<WindowFunctor>,gunzip_ns::dummy>...>
        auto ForwardWindow(const WindowFunctor& outputcopy)
        {
            return [&](std::uint_least16_t length, std::uint_fast32_t offset)
            {
                OutBytes(length);
                return outputcopy(length, offset);
            };
        }

        template<typename WindowFunctor, std::enable_if_t<DeflAbortable_WinFun<WindowFunctor>,gunzip_ns::dummy>...>
        auto ForwardWindow(const WindowFunctor& outputcopy)
        {
            return [&](std::uint_least16_t length, std::uint_fast32_t offset)
            {
                auto remain = outputcopy(length, offset);
                OutBytes(length - remain);
                return remain;
            };
        }
    };

    template<typename TrackType>
    struct SizeTracker: public SizeTracker_NoOutput, public SizeTracker_NoInput
    {
        inline int operator() (int returncode) const { return returncode; }
    };
    template<>
    struct SizeTracker<DeflateTrackOutSize>: public SizeTracker_NoInput, public SizeTracker_DoOutput
    {
        typedef std::pair<int,std::uint_fast64_t> result;
        inline result operator() (int returncode) const { return result{returncode,outsize}; }
    };
    template<>
    struct SizeTracker<DeflateTrackInSize>: public SizeTracker_NoOutput, public SizeTracker_DoInput
    {
        typedef std::pair<int,std::uint_fast64_t> result;
        inline result operator() (int returncode) const { return result{returncode,insize}; }
    };
    template<>
    struct SizeTracker<DeflateTrackBothSize>: public SizeTracker_DoInput, public SizeTracker_DoOutput
    {
        typedef std::pair<int, std::pair<std::uint_fast64_t,std::uint_fast64_t>> result;
        inline result operator() (int returncode) const { return result{returncode,std::make_pair(insize,outsize)}; }
    };

    #ifdef DO_DEFDB_DUMPING
    unsigned bitcounter=0;
    #endif
    struct DeflateBitCache
    {
        std::uint_least8_t BitCache = 0, BitCount = 0;

        template<bool Abortable, typename InputFunctor>
        std::uint_least64_t GetBits(InputFunctor&& input, unsigned numbits)
        {
            #ifdef DO_DEFDB_DUMPING
            bitcounter += numbits;
            #endif
            std::uint_fast64_t result = BitCache;
            if(numbits <= BitCount)
            {
                BitCount -= numbits;
                BitCache >>= numbits;
                result &= ((1u << numbits)-1); // 0-8
                return result;
            }
            for(unsigned acquired = BitCount; ; acquired += 8)
            {
                unsigned byte = input();
                if(Abortable && (byte & ~0xFFu))
                {
                    // Note: Throws away bits already eaten from BitCache
                    return ~std::uint_least64_t(0); // error
                }
                unsigned eat = numbits-acquired;
                byte &= 0xFF;
                if(eat <= 8)
                {
                    result |= ((std::uint_fast64_t)(byte & ((1u << eat)-1))) << acquired;
                    BitCount =       8-eat;
                    BitCache = byte >> eat;
                    return result;
                }
                result |= ((std::uint_fast64_t)(byte)) << acquired;
            }
        }

        template<bool Abortable, typename InputFunctor, unsigned A,unsigned B>
        std::uint_least32_t HuffRead(InputFunctor&& input,
                                     RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES,A,B>& tree)
        {
            int sum=0, cur=0, len=0;
            #ifdef DEFL_DO_HUFF_STATS
            static int maxlen = 0;
            #endif
            // Get more bits while code value is above sum
            do {
                auto p = GetBits<Abortable>(std::forward<InputFunctor>(input), 1);
                if(Abortable && !~p)
                {
                    // Note: Throws away progress already made traversing the tree
                    return ~std::uint_least32_t(0); // error flag
                }
                cur = (unsigned(cur) << 1) | unsigned(bool(p));
            #ifdef DEFL_DO_HUFF_STATS
                if(len > maxlen)
                {
                    maxlen = len;
                    std::fprintf(stderr, "maxlen access: %d (%d)\n", maxlen, (int)tree.Get(0 + len));
                }
            #endif
                auto v = tree.Get(0 + len++);
                sum += v;
                cur -= v;
            } while(cur >= 0);
            return tree.Get(15 + sum + cur);
        }
    };

    template<bool Backtrackable>
    struct DeflateState: public DeflateBitCache
    {
        std::uint_least8_t lencode = 0, num = 0; // used in DynTreeFunc

        // Lengths are in 0..15 range.
        RandomAccessArray<USE_BITARRAY_FOR_LENGTHS, 288, CeilLog2<16>> Lengths;     // 144 bytes
        // Length tree
        //   Values up to 288 in indexes 0-14.   (Table)  (255 is max observed in wild)
        //   Values up to 287 in indexes 15-302. (Trans)
        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+288, CeilLog2<289>> ltree; // 341->344 bytes
        // Distance tree
        //   Values up to 32 in indexes 0-14.  (Table)
        //   Values up to 31 in indexes 15-46. (Trans)
        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+32, CeilLog2<33>> dtree;   // 36->40 bytes

        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+32, CeilLog2<33>>& lltree = dtree;

        // Theoretical minimum memory use:
        //   (15*log2(289) + 288*log2(288))/8 = 309.45 bytes for ltree
        //   (15*log2(33)  + 32 *log2(32))/8  =  29.46 bytes for dtree
        //                                      144.00 bytes for lengths
        //                                total 482.91 bytes

        template<bool Abortable, typename InputFunctor, typename BacktrackFunctor>
        auto DynTreeFunc(InputFunctor&& input, std::uint_fast16_t length, BacktrackFunctor&& /*backtrack*/,
                         bool
                         #ifdef DO_DEFDB_DUMPING
                             dists
                         #endif
                         )
        {
            Lengths = {}; // clear at least length nibbles; easiest to clear it all
            bool error = false;
            for(std::uint_fast16_t code = 0; ; )
            {
                #ifdef DO_DEFDB_DUMPING
                unsigned bits_before=bitcounter;
                #endif
                if(!num)
                {
                    auto p = HuffRead<Abortable>(std::forward<InputFunctor>(input), lltree);
                    if(Abortable && !~p) { error = true; goto done; }
                    std::uint_least8_t what = p; // 0-18

                    if(!(what & 16))    { lencode = what * 0x11u;           what = 0x01; } // 1 times (what < 16) (use what, set prev)
                    else if(what < 17)  { lencode = (lencode >> 4) * 0x11u; what = 0x23; } // 3..6 (use prev)
                    else if(what == 17) { lencode = 0;                      what = 0x33; } // 3..10   (use 0, set prev)
                    else                { lencode = 0;                      what = 0x7B; } // 11..138 (use 0, set prev)

                    p = GetBits<Abortable>(std::forward<InputFunctor>(input), what >> 4); // 0, 2, 3 or 7 bits
                    #ifdef DO_DEFDB_DUMPING
                    if(what>>4)
                    {
                        char tmp[64]="[_]"; sprintf(tmp, "[%d]", int(bitcounter-bits_before));
                        fprintf(stderr, "%4s %cREP (%d times)\n", tmp, (lencode&0xF)?'L':'Z', p+(what&0xF));
                    }
                    #endif

                    if(Abortable && !~p) { error = true; goto done; }
                    num = p + (what & 0xF); // 1..138
                }

                #ifdef DO_DEFDB_DUMPING
                bool rep=num>1;
                #endif
                do {
                    #ifdef DO_DEFDB_DUMPING
                    char tmp[64]="[_]"; if(!rep) sprintf(tmp, "[%d]", int(bitcounter-bits_before));
                    if(code == 0x100)
                        fprintf(stderr, "%4s EofB CL (val:%2d)\n", tmp, int(lencode&0xF));
                    else if(dists)
                        fprintf(stderr, "%4s d_%02d CL (val:%2d)\n", tmp, int(code), int(lencode&0xF));
                    else if(code > 0x100)
                        fprintf(stderr, "%4s l_%02d CL (val:%2d)\n", tmp, int(code- 0x101), int(lencode&0xF));
                    else
                        fprintf(stderr, "%4s 0x%02X CL (val:%2d)\n", tmp, (int)code, int(lencode&0xF));
                    #endif

                    --num;
                    Lengths.QSet(code++, lencode & 0xF);
                    if(code == length) { goto done; }
                } while(num > 0);
            }
         done:;
            return [this,error](unsigned index) -> std::conditional_t<Abortable, int, unsigned char>
                   {
                       if(Abortable && error) return -1;
                       return Lengths.Get(index);
                   };
        }
    };

    template<>
    struct DeflateState<true>: public DeflateBitCache
    {
        // Length tree
        //   Values up to 288 in indexes 0-14.   (Table)  (255 is max observed in wild)
        //   Values up to 287 in indexes 15-302. (Trans)
        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+288, CeilLog2<289>> ltree; // 341->344 bytes

        // Distance tree
        //   Values up to 32 in indexes 0-14.  (Table)
        //   Values up to 31 in indexes 15-46. (Trans)
        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+32, CeilLog2<33>> dtree;   // 36->40 bytes

        // Length-lengths tree
        //   Values up to 19 in indexes 0-14.  (Table) (13 is max observed in wild)
        //   Values up to 18 in indexes 15-33. (Trans)
        RandomAccessArray<USE_BITARRAY_FOR_HUFFNODES, 15+19, CeilLog2<20>> lltree;  // 22->24 bytes

        // Theoretical minimum memory use:
        //   (15*log2(289) + 288*log2(288))/8 = 309.45 bytes for ltree
        //   (15*log2(33)  + 32 *log2(32))/8  =  29.46 bytes for dtree
        //   (15*log2(20)  + 19 *log2(19))/8  =  18.19 bytes for lltree
        //                                total 357.10 bytes

        std::uint_least8_t lencode, num; // used in DynTreeFunc
        std::uint_least8_t checkpoint_lencode, checkpoint_num;
        std::uint_least8_t checkpoint_BitCache, checkpoint_BitCount;

        template<bool Abortable, typename InputFunctor, typename BacktrackFunctor>
        auto DynTreeFunc(InputFunctor&& input, std::uint_fast16_t /*length*/, BacktrackFunctor&& backtrack,
                         bool
                         #ifdef DO_DEFDB_DUMPING
                             dists
                         #endif
                         )
        {
            // Create checkpoint
            checkpoint_lencode  = 0;
            checkpoint_num      = 0;
            checkpoint_BitCache = BitCache;
            checkpoint_BitCount = BitCount;
            backtrack(false);

            return [this,&input,&backtrack](unsigned index) -> std::conditional_t<Abortable, int, unsigned char>
            {
                if(index == 0)
                {
                    // Restore checkpoint
                    lencode  = checkpoint_lencode;
                    num      = checkpoint_num;
                    BitCache = checkpoint_BitCache;
                    BitCount = checkpoint_BitCount;
                    backtrack(true);
                }

                if(Abortable && (num==0xFF)) return -1;

                if(!num)
                {
                    auto p = HuffRead<Abortable>(std::forward<InputFunctor>(input), lltree);
                    if(Abortable && !~p) { num = 0xFF; return -1; } // If p== ~uint64_t()
                    std::uint_least8_t what = p; // 0-18

                    if(!(what & 16))    { lencode = what * 0x11u;           what = 0x01; } // 1 times (what < 16) (use what, set prev)
                    else if(what < 17)  { lencode = (lencode >> 4) * 0x11u; what = 0x23; } // 3..6 (use prev)
                    else if(what == 17) { lencode = 0;                      what = 0x33; } // 3..10   (use 0, set prev)
                    else                { lencode = 0;                      what = 0x7B; } // 11..138 (use 0, set prev)

                    p = GetBits<Abortable>(std::forward<InputFunctor>(input), what >> 4); // 0, 2, 3 or 7 bits

                    if(Abortable && !~p) { num = 0xFF; return -1; } // If p== ~uint64_t()
                    num = p + (what & 0xF); // 1..138
                }
                --num;
                return (lencode & 0xF);
            };
        }
    };

    struct DeflateWindow
    {
        unsigned char Data[gunzip_ns::MAX_WINDOW_SIZE];
        SmallestType<CeilLog2<gunzip_ns::MAX_WINDOW_SIZE>> Head = 0;
    };

    #ifdef DEFLATE_ALLOCATION_STATIC
    template<typename ObjectType>
    ObjectType& GetStaticObj()
    {
        static thread_local ObjectType obj;
        obj.~ObjectType();
        new(&obj) ObjectType();
        return obj;
    }
    #endif

    /* Values of Abortable:
     *   Input abortable  = &1
     *   Output abortable = &2
     *   Resumable        = &4
     *
     *   Input abortable    Output abortable   Resumable     Value
     *                no                  no          no     0
     *               yes                  no          no     1
     *                no                 yes          no     2
     *               yes                 yes          no     3
     *                                                       4 = invalid
     *               yes                  no         yes     5
     *                no                 yes         yes     6
     *               yes                 yes         yes     7
     */
    template<unsigned char Abortable,
             typename InputFunctor, typename OutputFunctor, typename WindowFunctor,
             typename BacktrackFunctor>
    int Gunzip(InputFunctor&& input,
               OutputFunctor&& output,
               WindowFunctor&& outputcopy,
               BacktrackFunctor&& backtrack)
    {
        using namespace gunzip_ns;

        typedef DeflateState<!std::is_same_v<remove_cvref_t<BacktrackFunctor>,dummy>> StateType;
#ifdef DEFLATE_ALLOCATION_AUTOMATIC
        StateType state;
#elif defined(DEFLATE_ALLOCATION_STATIC)
        auto& state = gunzip_ns::GetStaticObj<StateType>();
#elif defined(DEFLATE_ALLOCATION_DYNAMIC)
        std::unique_ptr<StateType> stateptr(new StateType);
        auto& state = *stateptr;
#endif

        // The following routines are macros rather than e.g. lambda functions,
        // in order to make them inlined in the function structure, and breakable/resumable.
	#define CONCAT(a, b) a##b

        // Bit-by-bit input routine
        #define DummyGetBits_(line,numbits) do { \
            auto CONCAT(pd,line) = state.template GetBits<bool(Abortable&Flag_InputAbortable)>(std::forward<InputFunctor>(input), numbits); \
            if((Abortable & Flag_InputAbortable) && !~CONCAT(pd,line)) return -2; \
        } while(0)
        #define DummyGetBits(numbits) DummyGetBits_(__LINE__, numbits)

        #define GetBits_(line,numbits, target) \
            auto CONCAT(pb,line) = state.template GetBits<bool(Abortable&Flag_InputAbortable)>(std::forward<InputFunctor>(input), numbits); \
            if((Abortable & Flag_InputAbortable) && !~CONCAT(pb,line)) return -2; \
            target = CONCAT(pb,line)
        #define GetBits(numbits, target) GetBits_(__LINE__, numbits, target)

        // Huffman tree read routine.
        #define HuffRead_(line, tree, target) \
            auto CONCAT(ph,line) = state.template HuffRead<bool(Abortable&Flag_InputAbortable)>(std::forward<InputFunctor>(input), tree); \
            if((Abortable & Flag_InputAbortable) && !~CONCAT(ph,line)) return -2; \
            target = CONCAT(ph,line)
        #define HuffRead(tree, target) HuffRead_(__LINE__, tree, target)

        #define Fail_If(condition) do { \
            /*assert(!(condition));*/ \
            if(condition) return -1; \
        } while(0)

        // Read stream header
        GetBits(16, std::uint_least16_t header);
        // ^ Read deflate header: method[4] ; winsize[4] ; checksum[8]
        if(header == 0x8B1F) // Is it actually a gzip header?
        {
            // Get format identifier, flags, MTIME, XFL and OS
            GetBits(64, header); Fail_If((header & 0xFF) != 8); // Format identifier should be 8
            if(header&0x0400) // Skip extra fields as indicated by FEXTRA
                { GetBits(16, std::uint_fast16_t q); DummyGetBits(8*q); }
            if(header&0x0800) for(;;) { GetBits(8, bool q); if(!q) break; } // NAME: Skip filename if FNAME was present
            if(header&0x1000) for(;;) { GetBits(8, bool q); if(!q) break; } // COMMENT: Skip comment if FCOMMENT was present
            if(header&0x0200) { DummyGetBits(16); }      // HCRC: Skip FCRC if was present
            outputcopy(0, 32768u); // GZIP always uses 32k window
        }
        else // No. Deflate header?
        {
            Fail_If((header & 0x208F) != 0x0008 || ((((header<<8)+(header>>8))&0xFFFF)%31) != 0);
            outputcopy(0, 256 << ((header >> 4) & 0xF)); // Preset dictionary (0x2000) is not supported
        }

        // Read compressed blocks
        for(;;)
        {
            GetBits(3, header);
            //fprintf(stderr, "header=%d\n", header);
            if(header & 4) // Dynamic block
            {
                Fail_If(header & 2);
                std::uint_least16_t nlen_ndist_ncode;
                GetBits(14, nlen_ndist_ncode);

                #define nlen  (((nlen_ndist_ncode >> 0u) & 0x1Fu) + 257u) // 257..288
                #define ndist (((nlen_ndist_ncode >> 5u) & 0x1Fu) + 1u)   // 1..32


                std::uint_least8_t ncode = ((nlen_ndist_ncode >> 10u) + 4u); // 4..19
                {std::uint_fast64_t lenlens; GetBits(ncode*3, lenlens);      // Max: 19*3 = 57 bits
                #ifdef DO_DEFDB_DUMPING
                fprintf(stderr, " [5] HLIT%5d (val:%d)\n [5] HDIST%4d (val:%d)\n [4] HCLEN%4d (val:%d)\n",
                    nlen,nlen-257, ndist,ndist-1, ncode,ncode-4);
                for(unsigned a=0; a<19; ++a)
                    for(unsigned b=0; b<19; ++b)
                        if(rshift(b) == 3*a)
                        {
                            if(a < ncode)
                                fprintf(stderr, " [3]%3d CLL (val: %d)\n", b, int((lenlens >> rshift(b)) & 7));
                            else
                                fprintf(stderr, " [_]%3d CLL (val: %d)\n", b, int((lenlens >> rshift(b)) & 7));
                        }
                #endif

                auto lltree_fun = [=](unsigned a) -> unsigned char { return (lenlens >> rshift(a)) & 7; };
                while(CreateHuffmanTree<bool(Abortable&Flag_InputAbortable)>("Len Lengths", state.lltree, 19, lltree_fun)) { return -2; }}

                {auto ltree_fun = state.template DynTreeFunc<bool(Abortable&Flag_InputAbortable)>(std::forward<InputFunctor>(input), nlen, std::forward<BacktrackFunctor>(backtrack), false);
                while(CreateHuffmanTree<bool(Abortable&Flag_InputAbortable)>("Dyn Lengths", state.ltree, nlen, ltree_fun)) { return -2; }}

                {auto dtree_fun = state.template DynTreeFunc<bool(Abortable&Flag_InputAbortable)>(std::forward<InputFunctor>(input), ndist, std::forward<BacktrackFunctor>(backtrack), true);
                while(CreateHuffmanTree<bool(Abortable&Flag_InputAbortable)>("Dyn Dists",   state.dtree, ndist, dtree_fun)) { return -2; }}

                #undef nlen
                #undef ndist
            }
            else           // Fixed block
            {
                if(header < 2) // Copy stored block data
                {
                    DummyGetBits(state.BitCount%8); // Go to byte boundary (discard a few bits)
                    GetBits(32, std::uint_least32_t a);
                    Fail_If(((a ^ (a >> 16)) & 0xFFFF) != 0xFFFF);
                    #ifdef DO_DEFDB_DUMPING
                    fprintf(stderr, "raw block of %d bytes (0x%X)\n", (unsigned short)a, a);
                    #endif
                    // Note: It is valid for (lower 16 bits of) "a" to be 0 here.
                    // It is sometimes used for aligning the stream to byte boundary.
                    while(a-- & 0xFFFF)
                    {
                        GetBits(8, unsigned char octet);
                        while(OutputHelper<bool(Abortable&Flag_OutputAbortable)>::output(output, octet)) { return -3; }
                    }
                    goto skipdef;
                }

                unsigned char (*ltree_fun)(unsigned) = [](unsigned n)->unsigned char{return (n<0x90 || n>=0x118) ? 8u : (n<0x100 ? 9u : 7u); };
                unsigned char (*dtree_fun)(unsigned) = [](unsigned  )->unsigned char{return 5u;};
                while(CreateHuffmanTree<false>("Stat Lengths", state.ltree, 288, ltree_fun)) { return -2; }
                while(CreateHuffmanTree<false>("Stat Dists",   state.dtree, 32,  dtree_fun)) { return -2; }
            }
            // Do actual deflating.
            for(;;)
            {
                #ifdef DO_DEFDB_DUMPING
                unsigned a=bitcounter;
                #endif
                HuffRead(state.ltree, std::uint_least16_t lencode); // 0..287
                if(!(lencode & -256)) // 0..255: literal byte
                {
                    #ifdef DO_DEFDB_DUMPING
                    {char tmp[64];sprintf(tmp,"[%d]",bitcounter-a); fprintf(stderr, "%4s %02X\n", tmp, lencode);}
                    #endif
                    while(OutputHelper<bool(Abortable&Flag_OutputAbortable)>::output(output, lencode)) { return -3; }
                }
                else if(!(lencode & 0xFF)) break; // 256=end
                else // 257..287: length code for backwards reference
                {
                    GetBits(lbits(lencode), std::uint_least16_t length); length += lbase(lencode);
                    {HuffRead(state.dtree, std::uint_least8_t distcode); // Read distance code (0..31)
                    {GetBits(dbits(distcode), std::uint_least32_t offset); offset += dbase(distcode);
                    #ifdef DO_DEFDB_DUMPING
                    {char tmp[64];sprintf(tmp,"[%d]",bitcounter-a); fprintf(stderr, "%4s (%d,%d)\n", tmp,length,offset);}
                    #endif
                    while(OutputHelper<bool(Abortable&Flag_OutputAbortable)>::outputcopy(outputcopy,length,offset)) { return -4; }}}
                }
            }
    skipdef:if(header & 1) break; // last block flag
        }
        // Note: after this, may come a checksum, and a trailer. Ignoring them.
        #undef GetBits
        #undef DummyGetBits
        #undef Fail_If
        #undef HuffRead
        return 0;
    }
}//ns


/*
`InputParams` may be one of the following sets of parameters:

* InputFunctor   input                                   `(5)` `(14)`
* InputIterator  begin                                   `(7)` `(14)`
* InputIterator  begin, InputIterator end                `(6)` `(14)`
* InputIterator  begin, SizeType length                  `(8)` `(14)`
* BidirectionalIterator begin, SizeType length           `(8)` `(15)`
* ForwardIterator       begin                            `(7)` `(14)`
* BidirectionalIterator begin                            `(7)` `(15)`
* RandomAccessIterator  begin                            `(7)` `(15)`
* ForwardIterator       begin, ForwardIterator       end `(6)` `(15)`
* BidirectionalIterator begin, BidirectionalIterator end `(6)` `(15)`
* RandomAccessIterator  begin, RandomAccessIterator  end `(6)` `(15)`

`OutputParams` may be one of the following sets of parameters:

* OutputFunctor        output                          `(1)` `(9)`
* OutputFunctor        output, WindowFunctor window    `(2)`
* OutputIterator       target                          `(9)`
* RandomAccessIterator target                                           `(10)`
* RandomAccessIterator target,   SizeType target_limit            `(3)` `(10)`
* RandomAccessIterator target,   RandomAccessIterator target_end  `(4)` `(10)`
*/

namespace gunzip_ns
{
    #ifdef DEFLATE_ALLOCATION_AUTOMATIC
        #define DeflDeclWindow gunzip_ns::DeflateWindow window;
    #elif defined(DEFLATE_ALLOCATION_STATIC)
        #define DeflDeclWindow auto& window = gunzip_ns::GetStaticObj<gunzip_ns::DeflateWindow>();
    #elif defined(DEFLATE_ALLOCATION_DYNAMIC)
        #define DeflDeclWindow std::unique_ptr<gunzip_ns::DeflateWindow> winptr(new gunzip_ns::DeflateWindow); \
                               auto& window = *winptr;
    #endif

    template<unsigned char code, typename I,typename O,typename C,typename B>
    auto DeflateDispatchFinal(I&& i, O&& o, C&& c, B&& b)
    {
        if constexpr(code & (Flag_TrackIn | Flag_TrackOut))
        {
            //fprintf(stderr, "both track flag\n");
            SizeTracker<DeflateTrackBothSize> tracker;
            return tracker(Gunzip<code & Flag_NoTrackFlagMask>
                (tracker.template ForwardInput<I>(i), tracker.template ForwardOutput<O>(o), tracker.template ForwardWindow<C>(c), std::forward<B>(b)));
        }
        else if constexpr(code & Flag_TrackIn)
        {
            //fprintf(stderr, "in track flag\n");
            SizeTracker<DeflateTrackInSize> tracker;
            return tracker(Gunzip<code & Flag_NoTrackFlagMask>
                (tracker.template ForwardInput<I>(i),std::forward<O>(o),std::forward<C>(c),std::forward<B>(b)));
        }
        else if constexpr(code & Flag_TrackOut)
        {
            //fprintf(stderr, "out track flag\n");
            SizeTracker<DeflateTrackOutSize> tracker;
            return tracker(Gunzip<code & Flag_NoTrackFlagMask>
                (std::forward<I>(i), tracker.template ForwardOutput<O>(o), tracker.template ForwardWindow<C>(c), std::forward<B>(b)));
        }
        else
        {
            //fprintf(stderr, "no track flag\n");
            return Gunzip<code & Flag_NoTrackFlagMask>(std::forward<I>(i),std::forward<O>(o),std::forward<C>(c),std::forward<B>(b));
        }
    }

    // One-parameter output dispatch:
    template<unsigned char code, typename BtFun, typename InFun, typename T1>
    auto DeflateOutputDispatch(BtFun&& bt, InFun&& infun, T1&& param1)
    {
        // Is param1 a random access iterator?
        if constexpr(is_random_iterator_v<T1>)
        {
            //fprintf(stderr, "random iterator\n");
            auto output     = [&](unsigned char l) { *param1 = l; ++param1; };
            auto outputcopy = [&](std::uint_least16_t length, std::uint_fast32_t offs)
            {
                /* length=0 means that offs is the size of the window. */
                for(; length--; ++param1) { *param1 = *(param1-offs); }
            };
            return DeflateDispatchFinal<code>(std::forward<InFun>(infun), output, outputcopy, std::forward<BtFun>(bt));
        }
        // Is param1 an output iterator?
        else if constexpr(is_output_iterator_v<T1>)
        {
            //fprintf(stderr, "output iterator\n");
            DeflDeclWindow
            auto output = [&](unsigned char l)
            {
                window.Data[window.Head++ % MAX_WINDOW_SIZE] = l;
                *param1 = l; ++param1;
            };
            auto outputcopy = [&](std::uint_least16_t length, std::uint_fast32_t offs)
            {
                /* length=0 means that offs is the size of the window. */
                for(; length>0; --length)
                {
                    unsigned char byte = window.Data[(window.Head - offs) % MAX_WINDOW_SIZE];
                    output(byte);
                }
                return false;
            };
            return DeflateDispatchFinal<code>(std::forward<InFun>(infun), output, outputcopy, std::forward<BtFun>(bt));
        }
        // param1 must be an output functor, then.
        else if constexpr(is_output_functor_v<T1>)
        {
            //fprintf(stderr, "output functor\n");
            DeflDeclWindow
            auto output = [&](unsigned char l)
            {
                window.Data[window.Head++ % MAX_WINDOW_SIZE] = l;
                return param1(l);
            };
            auto outputcopy = [&](std::uint_least16_t length, std::uint_fast32_t offs)
            {
                /* length=0 means that offs is the size of the window. */
                for(; length>0; --length)
                {
                    unsigned char byte = window.Data[(window.Head - offs) % MAX_WINDOW_SIZE];
                    if(OutputHelper<DeflAbortable_OutFun<T1>>::output(output, byte))
                        break;
                }
                return length;
            };
            return DeflateDispatchFinal
                <code | (DeflAbortable_OutFun<T1> ? Flag_OutputAbortable : 0)>
                (std::forward<InFun>(infun), output, outputcopy, std::forward<BtFun>(bt));
        }
        else
        {
            //fprintf(stderr, "unreached code 1\n");
            static_assert(code==0xFF, "Deflate: Unknown output parameter type");
        }
    }

    // Two-parameter output dispatch:
    template<unsigned char code, typename BtFun, typename InFun, typename T1, typename T2>
    auto DeflateOutputDispatch(BtFun&& bt, InFun&& infun, T1&& param1, T2&& param2)
    {
        if constexpr(std::is_same_v<remove_cvref_t<T2>, DeflateTrackNoSize>)
        {
            //fprintf(stderr, "no track flag...\n");
            return DeflateOutputDispatch<code> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(param1));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T2>, DeflateTrackInSize>)
        {
            //fprintf(stderr, "in track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackIn> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(param1));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T2>, DeflateTrackOutSize>)
        {
            //fprintf(stderr, "out track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackOut> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(param1));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T2>, DeflateTrackBothSize>)
        {
            //fprintf(stderr, "both track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackIn | Flag_TrackOut> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(param1));
        }
        // Are param1 and param2 both random access iterators?
        else if constexpr(std::is_same_v<T1,T2> && is_random_iterator_v<T1>)
        {
            //fprintf(stderr, "random iterator + random iterator\n");
            auto output = [&](unsigned char l)
            {
                if(param1 == param2) return true;
                *param1 = l; ++param1;
                return false;
            };
            auto outputcopy = [&](std::uint_least16_t length, std::uint_fast32_t offs)
            {
                /* length=0 means that offs is the size of the window. */
                for(; length > 0 && !(param1 == param2); --length, ++param1)
                {
                    *param1 = *(param1 - offs);
                }
                return length;
            };
            return DeflateDispatchFinal<code | Flag_OutputAbortable>(std::forward<InFun>(infun), output, outputcopy, std::forward<BtFun>(bt));
        }
        // Is param1 a random access iterator and param2 a size?
        else if constexpr(is_size_type_v<T2> && is_random_iterator_v<T1>)
        {
            //fprintf(stderr, "random iterator + size\n");
            typename std::iterator_traits<remove_cvref_t<T1>>::difference_type used{}, cap=param2;
            auto output = [&](unsigned char l)
            {
                if(used >= cap) return true;
                param1[used] = l; ++used;
                return false;
            };
            auto outputcopy = [&](std::uint_least16_t length, std::uint_fast32_t offs)
            {
                /* length=0 means that offs is the size of the window. */
                for(; length > 0 && used < cap; ++used, --length)
                {
                    param1[used] = param1[used - offs];
                }
                return length;
            };
            return DeflateDispatchFinal<code | Flag_OutputAbortable>(std::forward<InFun>(infun), output, outputcopy, std::forward<BtFun>(bt));
        }
        // Then, param1 must be an output functor and param2 a window functor.
        else if constexpr(is_output_functor_v<T1> && is_window_functor_v<T2>)
        {
            //fprintf(stderr, "output functor + window functor\n");
            return DeflateDispatchFinal
                <code | ( (DeflAbortable_OutFun<T1> && DeflAbortable_WinFun<T2>) ? Flag_OutputAbortable : 0 ) >
                (std::forward<InFun>(infun), std::forward<T1>(param1), std::forward<T2>(param2), std::forward<BtFun>(bt));
        }
        else
        {
            //fprintf(stderr, "unreached code 2\n");
            static_assert(code==0xFF, "Deflate: Unknown output parameter type");
        }
    }

    // Three-parameter output dispatch:
    template<unsigned char code, typename BtFun, typename InFun, typename T1, typename T2, typename T3>
    auto DeflateOutputDispatch(BtFun&& bt, InFun&& infun, T1&& p1, T2&& p2, T3)
    {
        if constexpr(std::is_same_v<remove_cvref_t<T3>, DeflateTrackNoSize>)
        {
            //fprintf(stderr, "no track flag...\n");
            return DeflateOutputDispatch<code> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(p1), std::forward<T2>(p2));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T3>, DeflateTrackInSize>)
        {
            //fprintf(stderr, "in track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackIn> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(p1), std::forward<T2>(p2));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T3>, DeflateTrackOutSize>)
        {
            //fprintf(stderr, "out track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackOut> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(p1), std::forward<T2>(p2));
        }
        else if constexpr(std::is_same_v<remove_cvref_t<T3>, DeflateTrackBothSize>)
        {
            //fprintf(stderr, "both track flag...\n");
            return DeflateOutputDispatch<code | Flag_TrackIn | Flag_TrackOut> (std::forward<BtFun>(bt), std::forward<InFun>(infun), std::forward<T1>(p1), std::forward<T2>(p2));
        }
        else
        {
            //fprintf(stderr, "unreached code 3\n");
            static_assert(code==0xFF, "Deflate: Mismatched parameters. Expected last parameter to be a DeflateTrack option.");
        }
    }

    // One or two parameter input dispatch:
    template<unsigned char code, typename BtFun, typename T1, typename T2, typename... T>
    auto DeflateInputDispatch(BtFun&& bt, T1&& param1, T2&& param2, T&&... args)
    {
        using namespace gunzip_ns;
        // Are param1 and param2 an input iterator pair?
        if constexpr(std::is_same_v<T1, T2> && is_input_iterator_v<T1>)
        {
            //fprintf(stderr, "input iterator + input iterator\n");
            auto inputfun = [&]() -> std::common_type_t<int, decltype(*param1)>
                            { if(param1 == param2) { return -1; } int r = *param1; ++param1; return r; };
            return DeflateOutputDispatch<code|Flag_InputAbortable>(std::forward<BtFun>(bt),  inputfun, std::forward<T>(args)...);
        }
        // Are param1 and param2 a pair of bidirectional input iterators (forward, bidir, random)?
        else if constexpr(std::is_same_v<T1, T2> && is_bidir_input_v<T1>)
        {
            //fprintf(stderr, "bidir input + bidir input\n");
            remove_cvref_t<T1> saved{param1};
            auto btfun    = [&](bool act) { if(act) param1 = saved; else saved = std::move(param1); };
            auto inputfun = [&]() -> std::common_type_t<int, decltype(*param1)>
                            { if(param1 == param2) { return -1; } int r = *param1; ++param1; return r; };
            return DeflateOutputDispatch<code|Flag_InputAbortable>(btfun, inputfun, std::forward<T>(args)...);
        }
        // Is param1 an input iterator and param2 a size?
        else if constexpr(is_size_type_v<T2> && is_input_iterator_v<T1>)
        {
            //fprintf(stderr, "input iterator + size\n");
            typename std::iterator_traits<remove_cvref_t<T1>>::difference_type remain{param2};
            auto inputfun = [&]() -> std::common_type_t<int, decltype(*param1)>
                            { if(!remain) return -1; --remain; int r = *param1; ++param1; return r; };
            return DeflateOutputDispatch<code|Flag_InputAbortable>(std::forward<BtFun>(bt),  inputfun, std::forward<T>(args)...);
        }
        // Is param1 a bidirectional input iterator (forward, bidir, random) and param2 a size?
        else if constexpr(is_size_type_v<T2> && is_bidir_input_v<T1>)
        {
            //fprintf(stderr, "bidir input + size\n");
            typename std::iterator_traits<remove_cvref_t<T1>>::difference_type remain{param2}, savestate{};
            auto btfun    = [&](bool act) { if(act) { param1 -= (savestate-remain); remain = savestate; } else savestate = remain; };
            auto inputfun = [&]() -> std::common_type_t<int, decltype(*param1)>
                            { if(!remain) return -1; --remain; int r = *param1; ++param1; return r; };
            return DeflateOutputDispatch<code|Flag_InputAbortable>(btfun, inputfun, std::forward<T>(args)...);
        }
        // Is param1 an input iterator?
        else if constexpr(is_input_iterator_v<T1>)
        {
            //fprintf(stderr, "input iterator\n");
            auto inputfun = [&]() -> std::remove_cv_t<decltype(*param1)> { auto r = *param1; ++param1; return r; };
            return DeflateOutputDispatch
                <code | ( is_abortable_input_type_v<remove_cvref_t<decltype(*param1)>> ? Flag_InputAbortable : 0 ) >
                (std::forward<BtFun>(bt),  inputfun, std::forward<T2>(param2), std::forward<T>(args)...);
        }
        // Is param1 a bidirectional input iterator (forward, bidir, random)?
        else if constexpr(is_bidir_input_v<T1>)
        {
            //fprintf(stderr, "bidir input\n");
            remove_cvref_t<T1> saved{param1};
            auto btfun    = [&](bool act) { if(act) param1 = saved; else saved = std::move(param1); };
            auto inputfun = [&]() -> std::remove_cv_t<decltype(*param1)> { auto r = *param1; ++param1; return r; };
            return DeflateOutputDispatch<code>(btfun, inputfun, std::forward<T2>(param2), std::forward<T>(args)...);
        }
        // param1 must be an input functor, then. Let's move on to param2 testing!
        else if constexpr(is_input_functor_v<T1>)
        {
            //fprintf(stderr, "input functor\n");
            return DeflateOutputDispatch
                <code | ( DeflAbortable_InFun<T1> ? Flag_InputAbortable : 0 ) >
                (std::forward<BtFun>(bt),  std::forward<T1>(param1), std::forward<T2>(param2), std::forward<T>(args)...);
        }
        else
        {
            //fprintf(stderr, "unreached code 0\n");
            static_assert(code==0xFF, "Deflate: Mismatched parameters. Expected something for an input.");
        }
    }
    #undef DeflDeclWindow
}


template<typename... T>
auto Deflate(T&&... args)
{
    return gunzip_ns::DeflateInputDispatch<0>(gunzip_ns::dummy{}, std::forward<T>(args)...);
}

#endif /* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

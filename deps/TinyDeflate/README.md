# TinyDeflate

See https://github.com/bisqwit/TinyDeflate for the original version.

A deflate/gzip decompressor, as a C++17 template function,
that requires minimal amount of memory to work.

    Terms of use: Zlib    
    Copyright © 2018 Joel Yliluoma

## Memory usage at aggressive settings (backtrackable input)

* 408 bytes of automatic storage for three huffman trees (384 elements, 5…9 bits each, 88 % space efficiency)
* 24 bytes of temporary automatic storage while a huffman tree is being generated (15 elements, 9 bits each, 66 % space efficiency)
* An assortment of automatic variables for various purposes (may be register variables, depending on the architecture and of the compiler wits)
* ABI mandated alignment losses

Total: 408 bytes minimum, 432+N bytes maximum

Theoretical minimum at 100 % efficiency: 357.1 + 15.32 ≃ 373 bytes (not yet attained by this library).

## Memory usage at aggressive settings (non-backtrackable input)

* 144 bytes of automatic storage for length tables (288 elements, 4 bits each, 100 % space efficiency)
* 384 bytes of automatic storage for two huffman trees (350 elements, 5…9 bits each, 88 % space efficiency)
* 24 bytes of temporary automatic storage while a huffman tree is being generated (15 elements, 9 bits each, 66 % space efficiency)
* An assortment of automatic variables for various purposes (may be register variables, depending on the architecture and of the compiler wits)
* ABI mandated alignment losses

Total: 528 bytes minimum, 552+N bytes maximum

Theoretical minimum at 100 % efficiency: 144 + 338.9 + 15.32 ≃ 499 bytes (not yet attained by this library).

## Memory usage at default settings (backtrackable input)

* 687 bytes of automatic storage for three huffman trees (52 % space efficiency)
* 30 bytes of temporary automatic storage while a huffman tree is being generated (53 % space efficiency)
* An assortment of automatic variables for various purposes (may be register variables, depending on the architecture and of the compiler wits)
* ABI mandated alignment losses

Total: 687 bytes minimum, 717+N bytes maximum

## Memory usage at default settings (non-backtrackable input)

* 288 bytes of automatic storage for length tables (50 % space efficiency)
* 653 bytes of automatic storage for two huffman trees (52 % space efficiency)
* 30 bytes of temporary automatic storage while a huffman tree is being generated (53 % space efficiency)
* An assortment of automatic variables for various purposes (may be register variables, depending on the architecture and of the compiler wits)
* ABI mandated alignment losses

Total: 941 bytes minimum, 971+N bytes maximum

## Tuning

To adjust the memory usage, there are three settings in gunzip.hh you can change:

| Setting name | 'false' memory use bytes | 'true' memory use bytes | 'true' performance impact
| ------------------------------------------- | ---:| ----:|--------------
| `USE_BITARRAY_TEMPORARY_IN_HUFFMAN_CREATION` |     30     |     24     | Negligible
| `USE_BITARRAY_FOR_LENGTHS`                   | 288 or   0 | 144 or   0 | Noticeable
| `USE_BITARRAY_FOR_HUFFNODES`                 | 653 or 687 | 384 or 408 | Significant
| **Total**                                    | 971 or 717 | 552 or 432 | _Plus alignment losses, callframes and spills_

In addition, if you neither decompress into a raw memory area nor supply your own window function,
32768 bytes of automatic storage is allocated for the look-behind window.

You can also change the memory allocation scheme:

| `#define` name | Meaning
| --- | ---
| `DEFLATE_ALLOCATION_AUTOMATIC` | Automatic allocation (usually stack)
| `DEFLATE_ALLOCATION_STATIC`    | Static `thread_local` allocation (memory remains allocated throughout the program, and different threads have their own copy of the data). Note that this scheme excludes running multiple decompressions in parallel, unless you do it in different threads.
| `DEFLATE_ALLOCATION_DYNAMIC`   | Storage duration is the same as with automatic allocation, but the `new` keyword is explicitly used (which usually means heap/bss allocation).

There is also a constant `MAX_WINDOW_SIZE`, which is 32768 by default,
but you can reduce it to use less memory for the automatically allocated
window in situations where one is allocated (see note 9 below).
Note that this value must not be smaller than the maximum backreference
distance used by your compressed data.

## Unrequirements

* No dynamic memory is allocated under any circumstances, unless your user-supplied functors do it, or you `#define DEFLATE_ALLOCATION_DYNAMIC`.
* Aside from assert() in assert.h and some template metaprogramming tools in type_traits, no standard library functions are used.
* No global variables.
* Compatible with -fno-exceptions -fno-rtti compilation.
* Option to compile without constant arrays.

## Rationale

* Embedded platforms (Arduino, STM32 etc).
* ROM hacking

## Caveats

* Decompressor only. Deflate and GZIP streams are supported.
* Slower than your average inflate function. The template uses densely bitpacked arrays, which require plenty of bit-shifting operations for every access.
* The code obviously performs best on 32-bit or 64-bit platforms. Platforms where 32-bit entities must be synthesized from a number of 8-bit entities are at a disadvantage.
* Decompressed data integrity is not verified. Any checksum fields are totally ignored.
* On most systems, automatic storage means ‘stack allocation’. Depending on your circumstances, you may want to change the memory allocation scheme. See the Tuning chapter for details.

## Definitions

```C++
struct DeflateTrackNoSize{};
struct DeflateTrackInSize{};
struct DeflateTrackOutSize{};
struct DeflateTrackBothSize{};

int/*exit status*/ Deflate(InputParams..., OutputParams...,  DeflateTrackNoSize = {});

std::pair<int/*exit status*/, std::uint_fast64_t/*number of input bytes consumed*/>
    Deflate(InputParams..., OutputParams...,  DeflateTrackInSize);   // (11)

std::pair<int/*exit status*/, std::uint_fast64_t/*number of output bytes generated*/>
    Deflate(InputParams..., OutputParams...,  DeflateTrackOutSize);  // (12)

std::pair<int/*exit status*/, std::pair<std::uint_fast64_t/*in size*/, std::uint_fast64_t/*out size*/>>
    Deflate(InputParams..., OutputParams...,  DeflateTrackBothSize); // (13)

// A counter for sizes is only allocated if explicitly requested
// by using one of the former three tracking overloads.
```

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


1) If the output functor (`output`) returns a `bool`, and the returned value is `true`, the decompression aborts with return value -3
without writing any more data.

2) If the output functor (`output`) returns a `bool`, and the returned value is `true`, the decompression aborts with return value -3
without writing any more data.
If the window function returns an integer type, and the returned value is other than 0, the decompression aborts with return value -4
without writing any more data.
If either the window function returns `void`, or the output functor does not return a `bool`, aborting on output-full will not be compiled.

3) If `target_limit` bytes have been written into `target` and the decompression is not yet complete, the decompression aborts with return value -3
without writing any more data.

4) If `target_begin == target_end`, the decompression aborts with return value -3
without writing any more data.

5) If the input functor (`input`) returns an integer type other than a `char`, `signed char`, or `unsigned char`,
and the returned value is smaller than 0 or larger than 255, the decompression aborts with return value -2
without reading any more data.

6) If `begin == end`, the decompression aborts with return value -2.

7) If the input iterator deferences into a value outside the 0 — 255 range, the decompression aborts with return value -2
without reading any more data.

8) If `length` bytes have been read from `begin` and the decompression is not yet complete, the decompression aborts with return value -2
without reading any more data.

9) A separate 32768-byte sliding window will be automatically and separately allocated for the decompression.

10) The output data buffer is assumed to persist during the call and doubles as the sliding window for the decompression.

11) The `first` field in the return value has the same meaning as the `int` type return value described earlier.    
The `second` field in the return value contains the number of bytes that were consumed from the input.

12) The `first` field in the return value has the same meaning as the `int` type return value described earlier.    
The `second` field in the return value contains the number of bytes that were written to the output.

13) The `first` field in the return value has the same meaning as the `int` type return value described earlier.    
The `second.first` field in the return value contains the number of bytes that were consumed from the input.    
The `second.second` field in the return value contains the number of bytes that were written to the output.

14) This method is non-backtrackable, and uses a bit more memory than the backtrackable ones.

15) This method is backtrackable, meaning that some bytes in the input may be read twice. It uses less memory than the non-backtrackable calls. 

### Tips

Some of these definitions may be ambiguous.
If you hit a compiler error, choose a different call method.
To help distinguish between (`InputIterator`,`RandomAccessIterator`,`RandomAccessIterator`)
and (`ForwardIterator`,`ForwardIterator`,`OutputIterator`), make sure the input iterators
are _const_.

If you do multiple decompression calls in your program in different spots,
it may be wise to make sure they all use the same type of parameters,
to avoid having to instantiate multiple copies of `Deflate()`.
Lambda functors are an offender in this respect, because each lambda has a
unique type even if their contents and calling conventions are identical.
In the worst case, you can use `std::function` to wrap your calls
into a common interface. Check out this video for more about this topic: https://www.youtube.com/watch?v=rUB5Hlm9AaQ

## Requirements

```C++
// An InputFunctor has the following prototype,
// wherein type1 is convertible into unsigned char:
type1 input()

// An OutputFunctor has one of the following two prototypes,
// wherein type1 can accept unsigned int parameters in range 0-255:
void output(type1 byte_to_output)
bool output(type1 byte_to_output)

// A WindowFunctor has one of the following two prototypes,
// wherein type1 can accept unsigned int parameters in range 0-258,
// and     type2 can accept unsigned int parameters:
void  outputcopy(type1 length, type2 offs)
type2 outputcopy(type1 length, type2 offs)

// An InputIterator must have at least the following operations defined,
// where type1 is convertible into unsigned char:
const type1& operator*() const
InputIterator& operator++()

// A OutputIterator must have at least the following operations defined,
// where type1 is convertible into unsigned char:
type1& operator*() const
OutputIterator& operator++()

// A ForwardIterator must have at least the following operations defined,
// where type1 is convertible into unsigned char:
const type1& operator*() const
ForwardIterator& operator++()
bool operator==(const ForwardIterator&) const

// A RandomAccessIterator must have at least the following operations defined,
// where type1 is convertible into unsigned char,
// and type2 is a signed integer type (may be negative):
type1& operator*()
type1& operator[] (type2)
RandomAccessIterator operator- (type2)
RandomAccessIterator& operator++()
bool operator==(const RandomAccessIterator&) const
```

## Example use:

Decompress the standard input into the standard output (uses 32 kB automatically allocated window):

```C++
    Deflate([]()                   { return std::getchar(); },
            [](unsigned char byte) { std::putchar(byte); });
    
    // Or more simply:
    
    Deflate(std::getchar, std::putchar);
```

Decompress an array containing gzipped data into another array that must be large enough to hold the result.
A window buffer will not be allocated.

```C++
    extern const char compressed_data[];
    extern unsigned char outbuffer[131072];
    
    Deflate(compressed_data+0, outbuffer+0);
```

Same as above, but with range checking for output, and reporting of written size:

```C++
    extern const char compressed_data[];
    extern unsigned char outbuffer[131072];
    
    auto result = Deflate(compressed_data+0, outbuffer+0, sizeof(outbuffer), DeflateTrackOutSize{});
    if(result.first != 0) std::fprintf(stderr, "Error %d\n", result.first);
    std::fprintf(stderr, "%u bytes written\n", unsigned(result.second));
```

Same as above, but with range checking for both input and output:

```C++
    extern const char compressed_data[];
    extern unsigned compressed_data_length;
    extern unsigned char outbuffer[131072];
    
    int result = Deflate(compressed_data+0, compressed_data_length, outbuffer, outbuffer + sizeof(outbuffer));
    if(result != 0) std::fprintf(stderr, "Error\n");
```

Decompress using a custom window function (the separate 32 kB window buffer will not be allocated):

```C++
    std::vector<unsigned char> result;
    
    Deflate(std::getchar,
            [&](unsigned byte)
            {
                result.push_back(byte);
            },
            [&](unsigned length, unsigned offset)
            {
                 if(!length)
                 {
                     // offset contains the maximum look-behind distance.
                     // You could use this information to allocate a buffer of a particular size.
                     // length=0 case is invoked exactly once before any length!=0 cases are.
                 }
                 while(length-- > 0)
                 {
                     result.push_back( result[result.size()-offset] );
                 }
            });
```

Same as above, but stop decompressing once 4096 bytes have been written:

```C++
    std::vector<unsigned char> result;
    
    Deflate(std::getchar,
            [&](unsigned byte)
            {
                if(result.size() >= 4096) return true;
                result.push_back(byte);
                return false;
            },
            [&](unsigned length, unsigned offset)
            {
                 if(!length)
                 {
                     // offset contains the maximum look-behind distance.
                     // You could use this information to allocate a buffer of a particular size.
                     // length=0 case is invoked exactly once before any length!=0 cases are.
                 }
                 for(; result.size() < 4096 && length > 0; --length)
                 {
                     result.push_back( result[result.size()-offset] );
                 }
                 return length;
            });
```

## Misnomer

Yes, I am aware that the project is technically named misleadingly.
This project implements the _inflate_ algorithm (decompression),
not _deflate_ (compression).

In my defense, the _compression format_ is called deflate. There is no _inflate_ format.
This library decompresses data that has been compressed with _deflate_.

Think name, not verb.


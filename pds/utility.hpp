/******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-15 Nicola Bonelli <nicola@pfq.io>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#pragma once
#include <cstdint>
#include <cstddef>
#include <tuple>

namespace pds
{
    template <typename Tp, Tp mask, int k>
    inline Tp swap_bits(Tp p)
    {
        Tp q = ((p>>k)^p) & mask;
        return p^q^(q<<k);
    }

    // Reversing bits in a 64-bit word.

    uint64_t reverse_bits(uint64_t n)
    {
        static constexpr uint64_t m0 = 0x5555555555555555LLU;
        static constexpr uint64_t m1 = 0x0300c0303030c303LLU;
        static constexpr uint64_t m2 = 0x00c0300c03f0003fLLU;
        static constexpr uint64_t m3 = 0x00000ffc00003fffLLU;

        n = ((n>>1)&m0) | (n&m0)<<1;
        n = swap_bits<uint64_t, m1, 4>(n);
        n = swap_bits<uint64_t, m2, 8>(n);
        n = swap_bits<uint64_t, m3, 20>(n);
        n = (n >> 34) | (n << 30);
        return n;
    }

    // Reversing bits in a word, basic interchange scheme.

    uint32_t reserve_bits(uint32_t x)
    {
        x = (x & 0x55555555) <<  1 | (x & 0xAAAAAAAA) >>  1;
        x = (x & 0x33333333) <<  2 | (x & 0xCCCCCCCC) >>  2;
        x = (x & 0x0F0F0F0F) <<  4 | (x & 0xF0F0F0F0) >>  4;
        x = (x & 0x00FF00FF) <<  8 | (x & 0xFF00FF00) >>  8;
        x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
        return x;
    }

    constexpr size_t
    rank(size_t value)
    {
        return (value == 0 ? 0 :
                value & 1  ? 1 :
                1 + rank(value >> 1));
    }

    constexpr size_t
    log2(size_t value)
    {
        return value == 1 ? 0 : 1 + log2(value >> 1);
    }

    //
    // make_tuple: like make_tuple but possibly accepts a fewer number of arguments.
    // Missing arguments are default constructed.
    //

    template <typename ...Ts>
    inline std::tuple<Ts...>
    make_tuple()
    {
        return std::make_tuple(Ts{}...);
    }

    template <typename T, typename ...Ts, typename X, typename ...Xs>
    inline std::tuple<T, Ts...>
    make_tuple(X && x, Xs&& ... xs)
    {
        return std::tuple_cat(std::make_tuple(std::forward<X>(x)),
                                   make_tuple<Ts...>(std::forward<Xs>(xs)...));
    }

    //
    // placeholder type used to disambiguate variadic forwarding
    // ctor from copy/move ctor
    //

    struct ctor_args_t { };
    constexpr ctor_args_t ctor_args = ctor_args_t();

}

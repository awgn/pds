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

#include <type_traits>
#include <cstdint>
#include <tuple>

namespace pds {

    template<typename T1, typename T2>
    constexpr typename std::common_type<T1, T2>::type
    gcd(T1 m, T2 n)
    {
        static_assert(std::is_integral<T1>::value, "gcd: arguments must be integers");
        static_assert(std::is_integral<T2>::value, "gcd: arguments must be integers");

        return m == 0 ? std::abs(n) : 
               n == 0 ? std::abs(m) : 
               gcd(n, m % n);
    }


    template <typename T>
    constexpr std::tuple<T, T, T>
    ext_euclid(T a, T b)
    {
        if (a == 0) {
            return  std::make_tuple(b, 0, 1);
        }
        else {
            auto r = ext_euclid(b % a, a);
            return std::make_tuple(std::get<0>(r),
                                   std::get<2>(r)-(b/a)*std::get<1>(r),
                                   std::get<1>(r));
        }
    }


    template <typename T>
    constexpr T inv_mod(T a, T n)
    {
        auto r = ext_euclid(a, n);
        return (n + std::get<1>(r)) % n;
    }


    template <int A, typename T>
    constexpr T mangling(T x)
    {
        static_assert(A % 2 == 1, "mangling: A must be an odd number!");
        return x * A;
    }

    template <int A, typename T>
    constexpr T demangling(T x)
    {
        static_assert(A % 2 == 1, "demangling: A must be an odd number!");
        constexpr auto n =  static_cast<uint64_t>(1)<<(sizeof(T)*8);
        return mangling<inv_mod(static_cast<uint64_t>(A), n)>(x);
    }


} // namespace pds


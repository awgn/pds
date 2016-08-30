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

#include <pds/utility.hpp>
#include <pds/hash.hpp>

#include <iostream>

#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>



namespace pds {

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Hyper LogLog Counter
    //
    // Flajolet, P.; Fusy, E.; Gandouet, O.; Meunier, F. (2007).
    //
    // "HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm".
    // AOFA ’07: Proceedings of the 2007 International Conference on the Analysis of Algorithms.
    //

    template <size_t M>
    struct static_alpha
    {
        static_assert(M >= 128, "static_alpha: bad value");
        static constexpr double value = 0.7213/(1.0 + 1.079/M);
    };

    template <> struct static_alpha<2>  { static constexpr double value = 0.351;  };
    template <> struct static_alpha<4>  { static constexpr double value = 0.532;  };
    template <> struct static_alpha<8>  { static constexpr double value = 0.625;  };
    template <> struct static_alpha<16> { static constexpr double value = 0.673;  };
    template <> struct static_alpha<32> { static constexpr double value = 0.697;  };
    template <> struct static_alpha<64> { static constexpr double value = 0.709;  };


    template <typename Tb, size_t M, typename Hash>
    struct hyperloglog
    {
        constexpr static size_t K = log2(M);
        constexpr static size_t L = hash_bitsize<Hash>::value;

        static_assert((M&(M-1)) == 0, "HLLC: groups (m) must be a power of two");
        static_assert(L > K+3,        "HLLC: the hash_bitsize must be greater than K");

        template <typename X = Hash>
        hyperloglog(X x = X())
        : m_(M)
        , hash_(x)
        { }

        //
        // hash and process the element:
        //

        template <typename T>
        void operator()(T const &elem)
        {
            auto h = Hash{}(elem);
            auto j = h & make_mask(K);
            auto v = h >> K;

            m_[j] = std::max<size_t>( m_[j], rank(v) );
        }

        //
        // return the estimated value E:
        //

        double cardinality() const
        {
            double c = 0.0;

            for(auto bucket : m_)
            {
                c += 1.0/std::pow(2, bucket);
            }

            double e = static_alpha<M>::value * M * M / c;

            if (e <= (2.5*M))
            {
                 auto v = std::count_if(std::begin(m_), std::end(m_),
                                        [](uint8_t bucket)
                                        {
                                            return bucket == 0;
                                        });
                 if (v != 0)
                 {
                     e = M * std::log(static_cast<double>(M)/v);
                 }
            }
            else
            {
                constexpr double exp2_L = exp2(hash_bitsize<Hash>::value); 

                if (e > exp2_L/30)
                {
                    e = -exp2_L * std::log(1.0 - e/exp2_L);
                }
            }

            return e;
        }

        //
        // merge from another counter
        //

        hyperloglog &
        operator+=(hyperloglog const &other)
        {
            for(size_t n = 0; n < M; n++)
                m_[n] = std::max(m_[n], other.m_[n]);
            return *this;
        }

        //
        // reset counter
        //

        void
        reset()
        {
            for(size_t n = 0; n < M; n++)
                m_[n] = 0;
        }


    private:

        std::vector<Tb> m_;
        Hash hash_;
    };


    template <typename Tb, size_t M,  typename Hash>
    inline hyperloglog<Tb, M, Hash> 
    operator+(hyperloglog<Tb, M, Hash> lhs, hyperloglog<Tb, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

}  // namespace pds

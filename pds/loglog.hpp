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
    // LogLog counter:
    //
    // Durand, M.; Flajolet, P. (2003). "LogLog counting of large cardinalities.".
    //
    // In G. Di Battista and U. Zwick. Lecture Notes in Computer Science. Annual European
    // Symposium on Algorithms (ESA03) 2832. Springer. pp. 605–617
    //

    template <typename T, size_t M = 1024, typename Hash = std::hash<T>>
    class loglog
    {
        static_assert((M&(M-1)) == 0, "LLC: groups (m) must be a power of two");

    public:

        constexpr static size_t K = log2(M);

        template <typename X = Hash>
        loglog(X x = X())
        : m_(M)
        , hash_(x)
        { }

        //
        // hash and process the element:
        //

        void operator()(T const &elem)
        {
            auto h = hash_(elem);
            auto j = h & ((1 << K) -1);
            auto v = h >> K;

            m_[j] = std::max<size_t>( m_[j], rank(v) );
        }

        //
        // return the estimated value E:
        //

        double cardinality() const
        {
            double sum = 0.0;

            for(auto bucket : m_)
                sum += bucket;

            return alpha(M) * M * std::exp2(sum/M);
        }

        //
        // merge from another counter
        //

        loglog &
        operator+=(loglog const &other)
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

        static double alpha(size_t value)
        {
            double gamma = std::tgamma(-1.0/value);
            double ratio = (1 - std::exp2(1.0/value))/std::log(2.0);
            return 1.0 / std::pow(gamma * ratio, value);
        }

        std::vector<uint8_t> m_;
        Hash hash_;
    };


    template <typename T, size_t M,  typename Hash>
    inline loglog<T, M, Hash> 
    operator+(loglog<T, M, Hash> lhs, loglog<T, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

}  // namespace pds

#pragma once
#include <iostream>

#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>

#include <pds/utils.hpp>


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

        loglog()
        : m_(M)
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

    private:

        static double alpha(size_t value)
        {
            double gamma = std::tgamma(-1.0/value);
            double ratio = (1 - std::exp2(1.0/value))/std::log(2.0);
            return 1.0 / std::pow(gamma * ratio, value);
        }

        Hash hash_;
        std::vector<uint8_t> m_;
        size_t rank_;
    };


    template <typename T, size_t M,  typename Hash>
    loglog<T, M, Hash> operator+(loglog<T, M, Hash> lhs, loglog<T, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

}  // namespace pds

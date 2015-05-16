#pragma once
#include <iostream>

#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>

#include <stream/utils.hpp>


namespace stream { namespace loglog {

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
    class counter
    {
        static_assert((M&(M-1)) == 0, "LLC: groups (m) must be a power of two");

    public:

        constexpr static size_t K = log2(M);

        counter()
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
        // merge two counters
        //

        counter &
        operator+=(counter const &other)
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
    counter<T, M, Hash> operator+(counter<T, M, Hash> lhs, counter<T, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

} // namespace loglog


namespace hyperloglog {

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

    template <> struct static_alpha<16> { static constexpr double value = 0.673;  };
    template <> struct static_alpha<32> { static constexpr double value = 0.697;  };
    template <> struct static_alpha<64> { static constexpr double value = 0.709;  };

    //
    // counter
    //

    template <typename T, size_t M = 1024, typename Hash = std::hash<T>>
    class counter
    {
        static_assert((M&(M-1)) == 0, "HLLC: groups (m) must be a power of two");

    public:

        constexpr static size_t K = log2(M);

        counter()
        : m_(M)
        { }

        //
        // hash and process the element:
        //

        void operator()(T const &elem)
        {
            auto h = hash_(elem);
            auto j = h & ((1ULL << K) -1);
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
                constexpr double exp2_32 = 1ULL<<32;

                if (e > exp2_32/30)
                {
                    e = -exp2_32 * std::log(1.0 - e/exp2_32);
                }
            }

            return e;
        }

        //
        // merge two counters
        //

        counter &
        operator+=(counter const &other)
        {
            for(size_t n = 0; n < M; n++)
                m_[n] = std::max(m_[n], other.m_[n]);
            return *this;
        }

    private:

        Hash hash_;
        std::vector<uint8_t> m_;
        size_t rank_;
    };


    template <typename T, size_t M,  typename Hash>
    counter<T, M, Hash> operator+(counter<T, M, Hash> lhs, counter<T, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

}  // namespace hloglog
}  // namespace stream

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


    template <typename T, size_t M = 1024, typename Hash = std::hash<T>>
    class hyperloglog
    {
        static_assert((M&(M-1)) == 0, "HLLC: groups (m) must be a power of two");

    public:

        constexpr static size_t K = log2(M);

        hyperloglog()
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

        Hash hash_;
        std::vector<uint8_t> m_;
        size_t rank_;
    };


    template <typename T, size_t M,  typename Hash>
    hyperloglog<T, M, Hash> operator+(hyperloglog<T, M, Hash> lhs, hyperloglog<T, M, Hash> const &rhs)
    {
        return lhs += rhs;
    }

}  // namespace pds

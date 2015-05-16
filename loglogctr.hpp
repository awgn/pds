#pragma once

#include <vector>
#include <functional>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <cstring>

namespace prob
{
    template <typename T, typename Hash = std::hash<T>>
    struct LogLogCounter
    {
        //
        // default ctor
        //

        LogLogCounter(size_t m_groups = 1024)
        : R_(m_groups)
        , arity_(0)
        {
            if ((m_groups & (m_groups-1)) != 0)
                throw std::runtime_error("LLC: groups (m) must be a power of 2");

            while (m_groups >>= 1)
                arity_++;
        }

        //
        // hash and store the element into the counter:
        //

        void operator()(T const &elem)
        {
            auto h = hash_(elem);

            auto idx = h & ((1 << arity_) -1);
            auto val = h >> arity_;

            R_[idx] = std::max<size_t>( R_[idx], ffsll(val) );
        }

        //
        // return the estimated value E:
        //

        size_t value() const
        {
            auto m = R_.size();
            if (m == 1)
                return std::exp2(R_.at(0));

            auto mean_R = static_cast<double>(std::accumulate(std::begin(R_), std::end(R_), 0.0)) / m;

            return alpha(m)*m*std::exp2(mean_R);
        }

        //
        // Max-merge operation on two LogLogCounter (of the same type and size).
        //

        LogLogCounter &
        merge(LogLogCounter const &other)
        {
            if (R_.size() != other.R_.size())
                throw std::runtime_error("LLC: max-merge on counters with different size");

            for(size_t n = 0; n < R_.size(); n++)
                R_[n] = std::max(R_[n], other.R_[n]);

            return *this;
        }

        //
        // utility functions... groups (m) and arity (k)
        // such that m = 2^k
        //

        size_t groups() const
        {
            return R_.size();
        }

        size_t arity() const
        {
            return arity_;
        }

    private:
        static double alpha(double m)
        {
            return std::pow(std::tgamma(-1.0/m)*(std::exp2(-1.0/m)-1.0)/std::log(2.0), -m);
        }

        Hash hash_;
        std::vector<size_t> R_;
        size_t arity_;
    };


    template <typename T, typename Hash>
    LogLogCounter<T, Hash> merge(LogLogCounter<T,Hash> lhs, LogLogCounter<T,Hash> const &rhs)
    {
        return lhs.merge(rhs);
    }

};

#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>

namespace pds { namespace stat
{
    // Mean Absolute Deviation...
    //
    struct MAD
    {
        template <typename T>
        double operator()(T forcast, T actual)
        {
            acc += std::abs(forcast-actual);
            n++;
            return acc/n;
        }

        double value() const
        {
            return acc/n;
        }

        size_t n = 0;
        double acc = 0;
    };

    // Normalised Root-Mean-Square deviation...
    //
    struct NRMSD
    {
        template <typename T>
        double operator()(T forcast, T actual)
        {
	    mean += forcast;
            acc += static_cast<double>((forcast-actual)*(forcast-actual));
            n++;
            return std::sqrt(acc/n)/(mean/n) * 100;
        }

        double value() const
        {
            return std::sqrt(acc/n)/(mean/n) * 100;
        }

        size_t n = 0;
        double acc = 0;
        double mean = 0;
    };

    // Mean Square Error...
    //
    struct MSE
    {
        template <typename T>
        double operator()(T forcast, T actual)
        {
            acc += (forcast-actual)*(forcast-actual);
            n++;
            return acc/n;
        }

        double value() const
        {
            return acc/n;
        }

        size_t n = 0;
        double acc = 0;
    };

    // Mean Absolute Percentage Error...
    //
    struct MAPE
    {
        template <typename T>
        double operator()(T forcast, T actual)
        {
            acc += std::abs(static_cast<double>(actual-forcast))/actual;
            n++;
            return acc/n * 100;
        }

        double value() const
        {
            return acc/n * 100;
        }

        size_t n = 0;
        double acc = 0;
    };

} // namespace stat
} // namespace pds

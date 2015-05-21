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

#include <cstddef>
#include <utility>
#include <vector>
#include <limits>
#include <tuple>
#include <numeric>
#include <algorithm>

#include "pds/utility.hpp"

namespace pds {

    //
    // Sketch data structure:
    //
    // Cormode, Graham (2009). "Count-min sketch" (PDF). Encyclopedia of Database Systems. Springer. pp. 511–516.
    //

    template <typename T, std::size_t W, typename ...Fs>
    struct sketch
    {
        sketch()
        : data_(sizeof...(Fs), std::vector<T>(W))
        , fs_(make_tuple<Fs...>())
        { }

        template <typename ...Hs>
        sketch(ctor_args_t, Hs && ...hs)
        : data_(sizeof...(Fs), std::vector<T>(W))
        , fs_(make_tuple<Fs...>(std::forward<Hs>(hs)...))
        { }

        //
        // update functions
        //

        template <typename Tp, typename Fun>
        void update_with(Tp const &data, Fun action)
        {
            apply(data, action, std::make_index_sequence<sizeof...(Fs)>());
        }

        template <typename Tp, typename Fun>
        void update_with(Tp const &data, Fun action) const
        {
            apply(data, action, std::make_index_sequence<sizeof...(Fs)>());
        }

        //
        // increment counters
        //

        template <typename Tp>
        void increment(Tp const &data)
        {
            update_with(data, [](T &ctr) { ++ctr; });
        }

        //
        // decrement counters
        //

        template <typename Tp>
        void decrement(Tp const &data)
        {
            update_with(data, [](T &ctr) { --ctr; });
        }

        //
        // count min estimation
        //

        template <typename Tp>
        T count(Tp const &data) const
        {
            T n = std::numeric_limits<T>::max();

            update_with(data, [&](T const &ctr) {
                n = std::min(n, ctr);
            });

            return n;
        }

        //
        // k-ary estimation
        //

        template <typename Tp>
        double estimate(Tp const &data) const
        {
            std::vector<double> va;

            double sum = std::accumulate(std::begin(data_[0]),
                                         std::end(data_[0]),
                                         size_t{0});

            update_with(data, [&](T const &ctr) {
                auto va_ = (ctr - sum/W)/(1.0 - 1.0/W);
                va.push_back(va_);
            });

            std::sort(std::begin(va), std::end(va));

            auto m = va.size()/2;

            return (va.size() & 1) ?
                    va.at(m) :
                   (va.at(m) + va.at(m-1))/2;
        }

        //
        // reset all counters in the sketch
        //

        void
        reset()
        {
            for(auto & v : data_)
                for(auto & e : v)
                    e = T{};
        }

        template <typename Fun>
        void for_all(Fun f)
        {
            for(auto & v : data_)
                for(auto & e : v)
                    f(e);
        }

        // return the size of the sketch
        //

        constexpr inline std::pair<size_t, size_t>
        size() const
        {
            return std::make_pair(sizeof...(Fs), W);
        }

        //
        // merge from another sketch
        //

        sketch &
        operator+=(sketch const &other)
        {
            for(size_t i = 0; i < sizeof...(Fs); ++i)
            {
                auto & lhs = data_[i];
                auto & rhs = other.data_[i];
                for(size_t j = 0; j < W; ++j)
                    lhs[j] += rhs[j];
            }
            return *this;
        }

        sketch &
        operator-=(sketch const &other)
        {
            for(size_t i = 0; i < sizeof...(Fs); ++i)
            {
                auto & lhs = data_[i];
                auto & rhs = other.data_[i];
                for(size_t j = 0; j < W; ++j)
                    lhs[j] -= rhs[j];
            }
            return *this;
        }

    private:

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>)
        {
            auto sink = { (action(data_[N][std::get<N>(fs_)(data) % W]),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>) const
        {
            auto sink = { (action(data_[N][std::get<N>(fs_)(data) % W]),0)... };
            (void)sink;
        }

        std::vector<std::vector<T>> data_;
        std::tuple<Fs...> fs_;
    };

    template <typename T, std::size_t W, typename ...Fs>
    sketch<T, W, Fs...> operator+(sketch<T, W, Fs...> lhs, sketch<T, W, Fs...> const &rhs)
    {
        return lhs += rhs;
    }

    template <typename T, std::size_t W, typename ...Fs>
    sketch<T, W, Fs...> operator-(sketch<T, W, Fs...> lhs, sketch<T, W, Fs...> const &rhs)
    {
        return lhs -= rhs;
    }

} // namespace pds

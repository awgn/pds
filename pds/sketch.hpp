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

    template <typename T, std::size_t W, typename ...Hs>
    struct sketch
    {
        sketch()
        : data_(sizeof...(Hs), std::vector<T>(W))
        { }

        //
        // foreach bucket... 
        //

        template <typename Tp, typename Fun>
        void foreach_bucket(Tp const &elem, Fun action)
        {
            foreach_(elem, action, std::make_index_sequence<sizeof...(Hs)>());
        }

        template <typename Tp, typename Fun>
        void foreach_bucket(Tp const &elem, Fun action) const
        {
            foreach_(elem, action, std::make_index_sequence<sizeof...(Hs)>());
        }

        //
        // increment buckets
        //

        template <typename Tp>
        void increment_buckets(Tp const &elem)
        {
            foreach_bucket(elem, [](T &bucket) { ++bucket; });
        }

        //
        // decrement buckets
        //

        template <typename Tp>
        void decrement_buckets(Tp const &elem)
        {
            foreach_bucket(elem, [](T &bucket) { --bucket; });
        }

        //
        // count min estimation
        //

        template <typename Tp>
        T count(Tp const &elem) const
        {
            T n = std::numeric_limits<T>::max();

            foreach_bucket(elem, [&](T const &bucket) {
                n = std::min(n, bucket);
            });

            return n;
        }

        template <typename Tp>
        std::vector<T>
        buckets(Tp const &elem) const
        {
            std::vector<T> ret;

            foreach_bucket(elem, [&](T const &bucket) 
            {
                ret.push_back(bucket);
            });

            return ret;
        }


        //
        // filter the keys whose the given predicate holds for each
        // bucket
        //
        
        template <typename Range, typename Predicate>
        auto filter(Range keys, Predicate pred) const
        {   
            std::vector<typename Range::value_type> ret;

            while(auto elem = keys())
            {
                bool resp = true;

                foreach_bucket(*elem, [&](T const &bucket) 
                {
                    resp &= pred(bucket);
                }); 

                if (resp)
                    ret.push_back(*elem);
            }

            return ret;
        }


        //
        // k-ary estimation
        //

        template <typename Tp>
        double estimate(Tp const &elem) const
        {
            std::vector<double> va;

            double sum = std::accumulate(std::begin(data_[0]),
                                         std::end(data_[0]),
                                         size_t{0});

            foreach_bucket(elem, [&](T const &bucket) {
                auto va_ = (bucket - sum/W)/(1.0 - 1.0/W);
                va.push_back(va_);
            });

            std::sort(std::begin(va), std::end(va));

            auto m = va.size()/2;

            return (va.size() & 1) ?
                    va.at(m) :
                   (va.at(m) + va.at(m-1))/2;
        }

        //
        // reset all buckets in the sketch
        //

        void
        reset()
        {
            for(auto & v : data_)
                for(auto & e : v)
                    e = T{};
        }

        template <typename Fun>
        void forall(Fun f)
        {
            for(auto & v : data_)
                for(auto & e : v)
                    f(e);
        }

        //
        // return the size of the sketch
        //

        constexpr inline std::pair<size_t, size_t>
        size() const
        {
            return std::make_pair(sizeof...(Hs), W);
        }

        //
        // merge from another sketch
        //

        sketch &
        operator+=(sketch const &other)
        {
            for(size_t i = 0; i < sizeof...(Hs); ++i)
            {
                auto & lhs = data_[i];
                auto & rhs = other.data_[i];
                for(size_t j = 0; j < W; ++j)
                    lhs[j] += rhs[j];
            }
            return *this;
        }

    private:
        
        template <typename Tp, typename Fun, size_t ...N>
        void cont_(Tp const &elem, Fun action, std::index_sequence<N...>)
        {
            bool run = true;
            auto cont = [&](T &bkt) {
                if (run)
                    run = action(bkt);
            };
            auto sink = { (cont(data_[N][utility::type_at<N,Hs...>{}(elem) % W]),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void cont_(Tp const &elem, Fun action, std::index_sequence<N...>) const
        {
            bool run = true;
            auto cont = [&](T const &bkt) {
                if (run)
                    run = action(bkt);
            };
            auto sink = { (cont(data_[N][utility::type_at<N,Hs...>{}(elem) % W]),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void foreach_(Tp const &elem, Fun action, std::index_sequence<N...>)
        {
            auto sink = { (action(data_[N][utility::type_at<N,Hs...>{}(elem) % W]),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void foreach_(Tp const &elem, Fun action, std::index_sequence<N...>) const
        {
            auto sink = { (action(data_[N][utility::type_at<N,Hs...>{}(elem) % W]),0)... };
            (void)sink;
        }

        std::vector<std::vector<T>> data_;
    };

    template <typename T, std::size_t W, typename ...Hs>
    inline sketch<T, W, Hs...> 
    operator+(sketch<T, W, Hs...> lhs, sketch<T, W, Hs...> const &rhs)
    {
        return lhs += rhs;
    }

} // namespace pds

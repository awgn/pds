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
#include <pds/tuple.hpp>
#include <pds/hash.hpp>

#include <cstddef>
#include <utility>
#include <vector>
#include <limits>
#include <tuple>
#include <numeric>
#include <algorithm>


namespace pds {

    //
    // utility meta-functions...
    //
    
    namespace details
    {
        template <template <typename...> class Fun, typename ...Hs> struct hash_coherence;

        template <template <typename ...> class Fun, typename H> 
        struct hash_coherence<Fun, H>
        {
            enum { value = true };
        };
        template <template <typename ...> class Fun, typename H1, typename H2, typename ...Hs> 
        struct hash_coherence<Fun, H1, H2, Hs...>
        {
            enum { value = (static_cast<size_t>(Fun<H1>::value) == 
                            static_cast<size_t>(Fun<H2>::value)) && hash_coherence<Fun, H2, Hs...>::value };
        };
    } 

    //
    // Sketch data structure:
    //
    // Cormode, Graham (2009). "Count-min sketch" (PDF). Encyclopedia of Database Systems. Springer. pp. 511–516.
    //

    template <typename T, std::size_t W, typename ...Hs>
    struct sketch
    {   
        static_assert(details::hash_coherence<pds::hash_rank, Hs...>::value,        "Sketch: all hash functions must have the same rank (number of hash component)!");
        static_assert(details::hash_coherence<pds::hash_bitsize, Hs...>::value,     "Sketch: all hash functions must have the same co-domain size!");
        static_assert((1ULL << pds::hash_bitsize<type_at_t<0, Hs...>>::value) == W, "Sketch: W and co-domain size mismatch!");

        template <typename ...Xs>
        sketch(Xs ... xs)
        : data_(sizeof...(Hs), std::vector<T>(W))
        , hash_(pds::make_tuple<Hs...>(xs...))
        { }

        //
        // foreach bucket... 
        //

        template <typename Tp, typename Fun>
        void foreach_bucket(Tp const &elem, Fun action)
        {
            continuation_(elem, [&](T &bkt) {
                            action(bkt);
                            return true;
                          }, std::make_index_sequence<sizeof...(Hs)>());
        }

        template <typename Tp, typename Fun>
        void foreach_bucket(Tp const &elem, Fun action) const
        {
            continuation_(elem, [&](T const &bkt) {
                            action(bkt);
                            return true;
                          }, std::make_index_sequence<sizeof...(Hs)>());
        }
 
        template <typename Tp, typename Fun>
        bool continuation_bucket(Tp const &elem, Fun pred)
        {
            return continuation_(elem, pred, std::make_index_sequence<sizeof...(Hs)>());
        }

        template <typename Tp, typename Fun>
        bool continuation_bucket(Tp const &elem, Fun pred) const
        {
            return continuation_(elem, pred, std::make_index_sequence<sizeof...(Hs)>());
        }
        
        //
        // foreach index... 
        //
 
        template <typename Fun>
        void foreach_index(std::vector<std::vector<size_t>> const &idx, Fun fun)
        {
            size_t i = 0;
            for(auto const & row : idx)
            {
                for (auto const & j : row)
                {
                    fun(data_[i][j]);   
                }
                i++;
            }
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

        //
        // given the element, return the corresponding buckets
        //
        
        template <typename Tp>
        auto buckets(Tp const &elem) const
        {
            std::vector<T> ret;

            foreach_bucket(elem, [&](T const &bucket) 
            {
                ret.push_back(bucket);
            });

            return ret;
        }
        
        //
        // return the index of buckets whose value holds the given predicate 
        //
        // return the indexes of buckets whose value holds the given predicate 
        //
        
        template <typename Fun>
        auto indexes(Fun pred) const
        {
            std::vector<std::vector<size_t>> ret;

            for(auto & v : data_) {
                std::vector<size_t> row;
                size_t c = 0;
                for(auto & e : v) {
                    if (pred(e))
                        row.push_back(c);
                    c++;
                }
                ret.push_back(std::move(row));
            }

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

            for(auto elem : keys)
            {
                if (continuation_bucket(elem, pred))
                    ret.push_back(elem);
            }

            return ret;
        }
        
        
        template <typename Iter, typename Predicate>
        auto filter(Iter it, Iter end, Predicate pred) const
        {   
            std::vector<decltype(*it)> ret;

            for(; it != end; ++it) 
            {
                if (continuation_bucket(*it, pred))
                    ret.push_back(*it);
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

        template <typename Tp, typename Fun, size_t ...N>
        bool continuation_(Tp const &elem, Fun action, std::index_sequence<N...>)
        {
            bool run = true;
            auto cont = [&](T &bkt) {
                if (run)
                    run = action(bkt);
            };
            auto sink = { (cont(data_[N][std::get<N>(hash_)(elem) % W]),0)... };
            (void)sink;
            return run;
        }

        template <typename Tp, typename Fun, size_t ...N>
        bool continuation_(Tp const &elem, Fun action, std::index_sequence<N...>) const
        {
            bool run = true;
            auto cont = [&](T const &bkt) {
                if (run)
                    run = action(bkt);
            };
            auto sink = { (cont(data_[N][std::get<N>(hash_)(elem) % W]),0)... };
            (void)sink;
            return run;
        }

        std::vector<std::vector<T>> data_;
        std::tuple<Hs...> hash_;
    };

    template <typename T, std::size_t W, typename ...Hs>
    inline sketch<T, W, Hs...> 
    operator+(sketch<T, W, Hs...> lhs, sketch<T, W, Hs...> const &rhs)
    {
        return lhs += rhs;
    }

} // namespace pds

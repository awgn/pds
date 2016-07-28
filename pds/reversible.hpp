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

#include <pds/sketch.hpp>
#include <pds/hash.hpp>

#include <vector>
#include <tuple>
#include <iostream>


namespace pds {

    //
    // Reversible: algorithm to reverse a sketch defined atop modular hash functions.
    //


    template <typename T>
    struct candidate
    {   
        candidate(T n = T{}, std::vector<std::vector<size_t>> ids = {})
        : value(n)
        , indices(std::move(ids))
        { }


        T value;
        std::vector<std::vector<size_t>> indices;
    };


    template <typename CharT, typename Traits, typename T>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, candidate<T> const& c)
    {
        out << "{ w:" << c.value << " idx:[ ";
        for(auto i : c.indices)
        {
            out << "[";
            for(auto j : i)
                out << j << ' ';
            out << "]";
        }
        return out << "] }"; 
    }


    template < size_t J
             , typename Sketch
             , typename Range>
    auto candidates( Sketch const &sketch
                   , Range const &words
                   , std::vector<std::vector<size_t>> const &buckets)
    {
        std::vector<candidate<typename Range::value_type>> ret;

        for(auto const &word: words)
        {
            size_t i = 0;
            std::vector<std::vector<size_t>> total_indices;

            pds::tuple_continue([&](auto &hash) { // row

                auto & hf = hash.template sub_hash<J>();
                auto & bidx = buckets[i]; 

                auto indices = hash.template sub_match<J>(hf(word), bidx);
                if (!indices.empty()) {
                    i++;
                    total_indices.push_back(std::move(indices));
                    return true;
                }

                return false;

            }, sketch.hash_);

            auto ts = std::tuple_size<decltype(sketch.hash_)>();
            if (i == ts)  {
                ret.emplace_back(static_cast<typename Range::value_type>(word), std::move(total_indices));
            }
        }

        return ret;
    }

    
    namespace detail {

        template < typename Sketch
                 , typename ... Ranges
                 , size_t ... I>
        auto all_candidates( Sketch const &sketch
                           , std::tuple<Ranges...> const &words
                           , std::vector<std::vector<size_t>> const &buckets
                           , std::index_sequence<I...>)
        {
            return std::make_tuple(
                    pds::candidates<I>( sketch
                                      , std::get<I>(words)
                                      , buckets)...);
        }

    } // namespace detail

    template < typename Sketch
             , typename ... Ranges>
    auto all_candidates( Sketch const &sketch
                       , std::tuple<Ranges...> const &words
                       , std::vector<std::vector<size_t>> const &buckets)
    {
        return detail::all_candidates( sketch
                                     , words
                                     , buckets
                                     , std::make_index_sequence<sizeof...(Ranges)>{});
    }                    


} // namespace pds

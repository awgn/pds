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
#include <pds/annotated.hpp>
#include <pds/hash.hpp>
#include <pds/tuple.hpp>

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace pds {
    
    using Indices = std::vector<std::vector<size_t>>;
}


namespace std {

    template <typename CharT, typename Traits>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, pds::Indices const& idx)
    {
        out << "[";
        for(auto i : idx)
        {
            out << "[";
            for(auto j : i)
                out << j << ' ';
            out << "]";
        }
        return out << "]";
    }
}


namespace pds {

    optional<Indices>
    merge_indices(Indices const &i1, Indices const &i2, size_t tolerance = 0)
    {
        Indices ids;

        size_t null = 0;
        for(size_t n = 0; n < i1.size(); n++)
        {
            std::vector<size_t> v;

            auto &v1 = i1.at(n);
            auto &v2 = i2.at(n);

            std::set_intersection(std::begin(v1), std::end(v1),
                                  std::begin(v2), std::end(v2), 
                                  std::back_inserter(v));

            if (v.empty())
                null++;

            if (null > tolerance)
                return nullopt;

            ids.push_back(std::move(v));
        }

        return make_optional(ids);
    }

    
    struct merge_annotated_
    {
        template <typename T1, typename T2, typename A>
        auto operator()(pds::annotated<T1, A> const &c1,
                        pds::annotated<T2, A> const &c2,
                        size_t tolerance = 0) const
        -> optional<pds::annotated<pds::cat_type_t<T1,T2>, A>>
        {
            auto ann = merge_indices(c1.info, c2.info, tolerance);
            if (ann) 
                return make_optional(annotate(pds::tuple_cat(c1.value, c2.value), *ann));
            return nullopt;
        }
    };

    auto constexpr merge_annotated = merge_annotated_{};


    template < size_t J
             , typename Sketch
             , typename Range>
    auto candidates( Sketch const &sketch
                   , Range const &words
                   , std::vector<std::vector<size_t>> const &buckets)
    {
        std::vector<annotated<typename Range::value_type, Indices>> ret;

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

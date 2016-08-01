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
#include <pds/tuple.hpp>

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

#include <experimental/optional>

namespace pds {

    using namespace std::experimental;

    //
    // annotated types... 
    //

    template <typename T>
    struct annotated
    {   
        annotated(T n = T{}, std::vector<std::vector<size_t>> ids = {})
        : value(n)
        , indices(std::move(ids))
        { }

        T value;
        std::vector<std::vector<size_t>> indices;
    };


    template <typename T>
    auto make_candidate(T const &value, std::vector<std::vector<size_t>> ids = {})
    {
        return annotated<T>(value, std::move(ids));
    }


    template <typename T1, typename T2>
    optional<pds::annotated<decltype(pds::cat(std::declval<T1>(), std::declval<T2>()))>>
    merge_candidates(pds::annotated<T1> const &c1, 
                     pds::annotated<T2> const &c2, size_t tolerance = 0)
    {
        std::vector<std::vector<size_t>> ids;

        size_t null = 0;
        for(size_t n = 0; n < c1.indices.size(); n++)
        {
            std::vector<size_t> v;

            auto &v1 = c1.indices[n];
            auto &v2 = c2.indices[n];

            std::set_intersection(std::begin(v1), std::end(v1),
                                  std::begin(v2), std::end(v2), 
                                  std::back_inserter(v));

            if (v.empty())
                null++;

            if (null > tolerance)
                return nullopt;

            ids.push_back(std::move(v));
        }

        return make_optional(make_candidate(pds::cat(c1.value, c2.value), ids));
    }


    template <typename CharT, typename Traits, typename T>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, annotated<T> const& c)
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

} // namespace pds

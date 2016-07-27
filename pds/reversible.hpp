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

namespace pds {

    //
    // Reversible: algorithm to reverse a sketch defined with modular hash functions.
    //


    template <typename T>
    struct candidate
    {
        T value;
    };


    template < size_t J
             , typename Sketch
             , typename Range>
    auto candidates( Sketch const &sketch
                   , Range const &words
                   , std::vector<std::vector<size_t>> const &buckets)
    {
        std::vector<candidate<typename Range::value_type>> ret;

        for(auto &word: words)
        {
            size_t i = 0;

            pds::tuple_continue([&](auto &hash) { // row

                auto & hf = std::get<J>(hash);
                auto & bi = buckets[i]; 

                if (hash.match_any(hf(word), bi)) {
                    i++;
                    return true;
                }

                return false;

            }, sketch.hash_);

        }

        return ret;
    }


} // namespace pds

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

#include <vector>

#include "pds/utility.hpp"


namespace pds {

    //
    // Bloom Filter data structure:
    //

    template <size_t M, typename ...Ks>
    struct bloom_filter
    {
        static_assert((M & 7) == 0, "bloom_filter size must be a multiple of 8");

        bloom_filter()
        : filter_(M >> 3)
        { }

        template <typename T>
        void set(T const &data)
        {
            update(data,
                   [](std::pair<uint8_t &, size_t > ref)
                   {
                        ref.first |= (1 << ref.second);
                   },
                   std::make_index_sequence<sizeof...(Ks)>());
        }

        template <typename T>
        bool is_set(T const &data)
        {
            bool r = true;

            update(data,
                   [&](std::pair<uint8_t &, size_t > ref)
                   {
                        r = r && static_cast<bool>(ref.first & (1 << ref.second));
                   }, std::make_index_sequence<sizeof...(Ks)>());

            return r;
        }

        void reset()
        {
            for(auto & b : filter_)
                b = 0;
        }

        bloom_filter &
        operator+=(bloom_filter const &other)
        {
            for(size_t i = 0; i < (M>>3);  ++i)
            {
                filter_[i] |= other.filter_[i];
            }
            return *this;
        }

    private:

        template <typename Tp, typename Fun, size_t ...N>
        void update(Tp const &data, Fun action, std::index_sequence<N...>)
        {
            auto sink = {
                (action([&, this]() -> std::pair<uint8_t &, size_t>
                        {
                            auto h = type_at<N, Ks...>{}(data);
                            return std::pair<uint8_t &, size_t> { filter_[h % (M >> 3)], h & 7 };

                        }() ),0)... };
            (void)sink;
        }

        std::vector<uint8_t> filter_;
    };

    template <size_t M, typename ...Ks>
    inline bloom_filter<M, Ks...> operator+(bloom_filter<M, Ks...> lhs, bloom_filter<M, Ks...> const &rhs)
    {
        return lhs += rhs;
    }
}

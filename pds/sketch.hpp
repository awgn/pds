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

#include "pds/utils.hpp"

namespace pds {

    //
    // placeholder type used to disambiguate variadic forwarding
    // ctor from copy/move ctor
    //

    struct ctor_args_t { };
    constexpr ctor_args_t ctor_args = ctor_args_t();

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
        // min-count
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

    private:

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>)
        {
            auto sink { (action(data_.at(N).at(std::get<N>(fs_)(data) % W)),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>) const
        {
            auto sink { (action(data_.at(N).at(std::get<N>(fs_)(data) % W)),0)... };
            (void)sink;
        }

        std::vector<std::vector<T>> data_;
        std::tuple<Fs...> fs_;
    };

} // namespace pds

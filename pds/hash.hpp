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

#include <pds/tuple.hpp>
#include <pds/utility.hpp>

#include <functional>


namespace pds {

    template <typename Hash, size_t N = 8>
    struct ModularHash
    {
        ModularHash(Hash h = Hash())
        : hash_(h)
        { }


        template <typename ...Ts>
        auto 
        operator()(std::tuple<Ts...> const &value) const
        {
            static_assert(sizeof...(Ts)* N <= 64, "ModularHash: too many components/hash too large (max. 64bit)");

            uint64_t ret = 0;

            pds::tuple_foreach([&](auto &elem) 
            {
                ret <<= N;
                ret |= (hash_(elem) & make_mask(N));
            }, value);

            return ret;
        }

        template <size_t I, typename ...Ts>
        auto partial(std::tuple<Ts...> const &value)
        {
            return hash_(std::get<I>(value)) & make_mask(N);
        }


    private:

        Hash hash_;
    };


} // namespace pds

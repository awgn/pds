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

#include <cstdint>
#include <tuple>
#include <vector>
#include <stdexcept>

namespace pds {

    //
    // implementation details...
    //

    namespace details
    {
        template <typename Fun, typename TupleT, size_t ...N>
        void tuple_foreach(Fun fun, TupleT &&tup, std::index_sequence<N...>)
        {
            std::initializer_list<bool> sink {(fun(std::get<N>(std::forward<TupleT>(tup))), true)...};
            (void)sink;
        }
        
        template <typename Fun, typename TupleT, size_t ...N>
        void tuple_foreach_index(Fun fun, TupleT &&tup, std::index_sequence<N...>)
        {
            std::initializer_list<bool> sink {(fun(std::integral_constant<size_t,N>{}, std::get<N>(std::forward<TupleT>(tup))), true)...};
            (void)sink;
        }
   }
    
    //
    // index_tuple: create indexes for a tuple
    //

    template <typename TupleT>
    using index_tuple = std::make_index_sequence<std::tuple_size<std::decay_t<TupleT>>::value>;

    //
    // tuple_foreach...
    //
    
    template <typename Fun, typename TupleT>
    void tuple_foreach(Fun fun, TupleT &&tup)
    {
        return details::tuple_foreach(fun, std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }
    
    template <typename Fun, typename TupleT>
    void tuple_foreach_index(Fun fun, TupleT &&tup)
    {
        return details::tuple_foreach_index(fun, std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }

    //
    // hash_tuple...
    //
    
    template <typename Hash, typename ...Ts>
    uint64_t 
    hash_tuple(Hash fun, std::tuple<Ts...> const &tup, int bits)
    {
        uint64_t h = 0;
 
        if ((bits * sizeof...(Ts)) > sizeof(h)*8)
            throw std::runtime_error("hash_fold: too many hash values or hash size too large");

        if (bits == 0)
            throw std::runtime_error("hash_fold: hash component bits cannot be 0");

        pds::tuple_foreach([&](auto &elem) {
                h <<= bits;
                h |= (fun(elem) & ((1 << bits)-1));
        }, tup);

        return h;
    }

} // namespace pds



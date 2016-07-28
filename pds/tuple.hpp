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
        
        template <typename Fun, typename TupleT, size_t ...N>
        void tuple_continue(Fun fun, TupleT &&tup, std::index_sequence<N...>)
        {
            bool run = true;
            auto cont = [&](auto &elem) {
                if (run)
                    run = fun(elem);
            };
            auto sink = {(cont(std::get<N>(std::forward<TupleT>(tup))), 0)...};
            (void)sink;
        }
        
        template <typename Fun, typename TupleT, size_t ...N>
        void tuple_continue_index(Fun fun, TupleT &&tup, std::index_sequence<N...>)
        {
            bool run = true;
            auto cont = [&](auto I, auto &elem) {
                if (run)
                    run = fun(I, elem);
            };
            auto sink = {(cont(std::integral_constant<size_t,N>{}, std::get<N>(std::forward<TupleT>(tup))), true)...};
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
    
    
    template <typename Fun, typename TupleT>
    void tuple_continue(Fun fun, TupleT &&tup)
    {
        return details::tuple_continue(fun, std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }
    
    template <typename Fun, typename TupleT>
    void tuple_continue_index(Fun fun, TupleT &&tup)
    {
        return details::tuple_continue_index(fun, std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }
    
    //
    // make_tuple: like make_tuple but possibly accepts a fewer number of arguments.
    // Missing arguments are default constructed.
    //

    template <typename ...Ts>
    inline std::tuple<Ts...>
    make_tuple()
    {
        return std::make_tuple(Ts{}...);
    }

    template <typename T, typename ...Ts, typename X, typename ...Xs>
    inline std::tuple<T, Ts...>
    make_tuple(X && x, Xs&& ... xs)
    {
        return std::tuple_cat(std::make_tuple(std::forward<X>(x)),
                                   make_tuple<Ts...>(std::forward<Xs>(xs)...));
    }

    // 
    // tuple is streamable...
    //

    template <typename CharT, typename Traits, typename ...Ts>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, std::tuple<Ts...> const& tup)
    {
        size_t n = 0;
        out << '(';
        tuple_foreach([&](auto &elem) {
            out << elem;
            if (++n < sizeof...(Ts))
                out << ' ';
        }, tup);

        return out << ')';
    }


} // namespace pds



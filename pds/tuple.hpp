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
        
        
        template <typename Fun, typename T, typename TupleT>
        auto tuple_fold(Fun, T acc, TupleT &&, std::index_sequence<>)
        {
            return acc;
        }
        template <typename Fun, typename T, typename TupleT, size_t N, size_t ...Ns>
        auto tuple_fold(Fun fun, T acc, TupleT &&tup, std::index_sequence<N, Ns...>)
        {
            auto acc1 = fun(std::move(acc), std::get<N>(tup));
            return tuple_fold(fun, std::move(acc1), std::forward<TupleT>(tup), std::index_sequence<Ns...>{}); 
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //
    // index_tuple: create indexes for a tuple
    //

    template <typename TupleT>
    using index_tuple = std::make_index_sequence<std::tuple_size<std::decay_t<TupleT>>::value>;
    
    template <typename TupleT>
    using index_tuple_1 = std::make_index_sequence<std::tuple_size<std::decay_t<TupleT>>::value-1>;

    //
    // is_tuple trait
    //

    template <typename T>
    struct is_tuple
    {
        enum { value = false };
    };
    template <typename ...Ts>
    struct is_tuple<std::tuple<Ts...>>
    {
        enum { value = true };
    };
    
    //
    // generic tuple_cat...
    //
        
    template <typename T1, typename T2>
    auto tuple_cat(T1 const &t1, T2 const &t2)
    {
        return std::make_tuple(t1,t2);
    }

    template <typename ...T1, typename T2>
    auto tuple_cat(std::tuple<T1...> const &t1, T2 const &t2)
    {
        return std::tuple_cat(t1, std::make_tuple(t2));
    }

    template <typename T1, typename ...T2>
    auto tuple_cat(T1 const &t1, std::tuple<T2...> const &t2)
    {
        return std::tuple_cat(std::make_tuple(t1), t2);
    }
    template <typename ...T1, typename ...T2>
    auto tuple_cat(std::tuple<T1...> const &t1, std::tuple<T2...> const &t2)
    {
        return std::tuple_cat(t1,t2);
    }

    //
    // tuple_cat_type metafunction
    //

    template <typename ...Ts>
    struct tuple_cat_type;

    template <typename T1, typename T2>
    struct tuple_cat_type<T1,T2>
    {
        using type = decltype(tuple_cat(std::declval<T1>(), std::declval<T2>())); 
    };
    template <typename T1, typename T2, typename ...Ts>
    struct tuple_cat_type<T1,T2,Ts...>
    {
        using type = tuple_cat_type<
                        typename tuple_cat_type<T1, T2>::type, Ts...>;
    };

    template <typename ...Ts>
    using tuple_cat_type_t = typename tuple_cat_type<Ts...>::type;

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
    // tuple_foreach...
    //
    
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
    // tuple_tail...
    //

    template <typename T>
    auto tuple_tail(std::tuple<T> const &)
    {
        return std::tuple<>{};
    }
    template <typename T1, typename T2, typename ...Ts>
    auto tuple_tail(std::tuple<T1, T2, Ts...> const &t)
    {
        std::tuple<T2, Ts...> ret;

        tuple_foreach_index([&](auto Idx, auto &ret_i) {
            size_t constexpr const I = decltype(Idx)::value;
            ret_i = std::get<I+1>(t);
        }, ret);

        return ret;
    }

    // 
    // tuple_fold...
    //

    template<typename Fun, typename T, typename TupleT> 
    auto tuple_fold(Fun fun, T value, TupleT &&tup)
    {
        return details::tuple_fold(fun, std::move(value), std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }
    
    template<typename Fun, typename TupleT> 
    auto tuple_fold1(Fun fun, TupleT &&tup)
    {
        return details::tuple_fold(fun, std::get<0>(tup), tuple_tail(tup), index_tuple_1<TupleT>{});
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
    // make_flat_tuple
    //
    
    inline auto make_flat_tuple(std::tuple<> const &)
    {
        return std::tuple<>{};      
    }
    template <typename T, typename ...Ts>
    inline auto make_flat_tuple(std::tuple<T, Ts...> const &t)
    {
        return tuple_cat(std::get<0>(t), make_flat_tuple(tuple_tail(t)));
    }

} // namespace pds


namespace std
{
    // 
    // tuple is streamable...
    //

    template <typename CharT, typename Traits, typename ...Ts>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, std::tuple<Ts...> const& tup)
    {
        size_t n = 0;
        out << '(';
        pds::tuple_foreach([&](auto &elem) {
            out << elem;
            if (++n < sizeof...(Ts))
                out << ' ';
        }, tup);

        return out << ')';
    }
}



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

    template <size_t N, typename ...Hs>
    struct ModularHash
    {
        template <typename ...Xs>
        ModularHash(Xs...xs)
        : hash_(pds::make_tuple<Hs...>(xs...))
        { }


        template <typename ...Ts>
        auto 
        operator()(std::tuple<Ts...> const &value) const
        {
            static_assert(sizeof...(Ts)* N <= 64, "ModularHash: too many components/hash too large (max. 64bit)");

            uint64_t ret = 0;

            pds::tuple_foreach_index([&](auto I, auto &elem) 
            {
                constexpr auto i = decltype(I)::value;

                ret <<= N;
                ret |= (std::get<i>(hash_)(elem) & make_mask(N));

            }, value);

            return ret;
        }

        template <size_t J, typename HashValue>
        std::vector<size_t>
        sub_match(HashValue value, std::vector<size_t> const &idx) const
        {
            std::vector<size_t> ret;

            auto h = value & make_mask(N);
            for(auto i : idx)
            {
                if (h == ((i >> (N*J)) & make_mask(N)))
                    ret.push_back(i);
            }

            return ret;
        }


        template <size_t I>
        decltype(auto) 
        sub_hash() const
        {
            return std::get<I>(hash_);
        }

    private:

        std::tuple<Hs...> hash_;
    };



    template <size_t N, typename H>
    struct FoldHash
    {
        FoldHash(H h = H())
        : hash_(h)
        { }

        template <typename T>
        auto operator()(T const &value) const
        {
            return hash_(value) & make_mask(N);
        }

    private:
        H hash_;

    };



#if 0
    //
    // is_modular, is_modular_v
    //

    template <typename T>
    struct is_modular
    {
        enum { value = false };
    };

    template <size_t N, typename ...Hs>
    struct is_modular<ModularHash<N, Hs...>>
    {
        enum { value = true };
    };

    template <typename T>
    constexpr bool is_modular_v = is_modular<T>::value;

#endif

    //
    // rank
    //

    template <typename T>
    struct hash_rank
    {
        enum : size_t { value = 1 };
    };
    template <size_t N, typename ...Hs>
    struct hash_rank<ModularHash<N, Hs...>>
    {
        enum : size_t { value = sizeof...(Hs) };
    };

    //
    // codomain_size<Hash>
    //

    template <typename H>
    struct hash_codomain_size
    {
        enum : size_t { value = sizeof(H{}(0)) * 8} ;
    };
    template <size_t N, typename H>
    struct hash_codomain_size<FoldHash<N, H>>
    {
        enum : size_t { value = N };
    };
    template <size_t N, typename ...Hs>
    struct hash_codomain_size<ModularHash<N, Hs...>>
    {
        enum : size_t { value = N* sizeof...(Hs) };
    };

    //
    // Hash functions...
    // 

    struct H1
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            return static_cast<uint32_t>(value);
        }
    };

    struct H2
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x >> 8)^ (x >> 16) ^ (x >> 24);
        }
    };

    struct H3
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x >> 5)^ (x >> 11) ^ (x >> 23);
        }
    };
    
    struct H4
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x >> 9)^ (x >> 13) ^ (x >> 19);
        }
    };
    
    struct H5
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x >> 3)^ (x >> 11) ^ (x >> 22);
        }
    };

    struct H6
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x >> 5)^ (x >> 15) ^ (x >> 20);
        }
    };

} // namespace pds

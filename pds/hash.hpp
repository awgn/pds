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
#include <pds/type_traits.hpp>
#include <pds/utility.hpp>

#include <functional>


namespace std
{
    template <typename ...Ts>
    struct hash<std::tuple<Ts...>>
    {
        size_t operator()(std::tuple<Ts...> const &t) const
        {
            size_t ret = 0;

            pds::tuple_foreach([&](auto const &elem)
            {
                auto h = std::hash<std::decay_t<decltype(elem)>>{};
                ret ^= h(elem);
            }, t);

            return ret;
        }
    };
}

namespace pds {

    
    template <typename T>                           struct hash_bitsize;
    template <typename ...Ts>                       struct hash_total_bitsize;
    template <size_t N, typename H, typename ...Hs> struct hash_offset;


    template <typename ...Hs> 
    struct ModularHash
    {
        constexpr static size_t total_size = hash_total_bitsize<Hs...>::value;
        
        template <typename ...Xs>
        ModularHash(Xs...xs)
        : hash_(pds::make_tuple<Hs...>(xs...))
        { }


        template <typename ...Ts>
        auto operator()(std::tuple<Ts...> const &value) const
        {
            static_assert(total_size <= 64, "ModularHash: too many components/hash too large (max. 64bit)");

            uint64_t ret = 0;

            pds::tuple_foreach_index([&](auto Idx, auto &elem) 
            {
                constexpr auto I = decltype(Idx)::value;
                ret |= ((std::get<I>(hash_)(elem) & make_mask(hash_bitsize<type_at_t<I, Hs...>>::value)) << (hash_offset<I, Hs...>::value));

            }, value);

            return ret;
        }

        template <size_t J, typename HashValue>
        std::vector<size_t>
        sub_match(HashValue value, std::vector<size_t> const &idx) const
        {
            std::vector<size_t> ret;

            auto h = value & make_mask(hash_bitsize<type_at_t<J, Hs...>>::value);
            for(auto i : idx)
            {
                if (h == ((i >> (hash_offset<J, Hs...>::value)) & make_mask(hash_bitsize<type_at_t<J, Hs...>>::value)))
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


    //
    // fold a hash function to the number of bit:
    //
    // e.g.: HashFold<5, MyHashFunction>
    //
    
    template <size_t N, typename Hash>
    struct HashFold
    {
        HashFold(Hash h = Hash())
        : hash_(std::move(h))
        { }

        template <typename T>
        auto operator()(T const &value) const
        {
            return hash_(value) & make_mask(N);
        }

    private:
        Hash hash_;
    };

    #define BIT_1(x)  pds::HashFold<1,x>
    #define BIT_2(x)  pds::HashFold<2,x>
    #define BIT_3(x)  pds::HashFold<3,x>
    #define BIT_4(x)  pds::HashFold<4,x>
    #define BIT_5(x)  pds::HashFold<5,x>
    #define BIT_6(x)  pds::HashFold<6,x>
    #define BIT_7(x)  pds::HashFold<7,x>
    #define BIT_8(x)  pds::HashFold<8,x>
    #define BIT_9(x)  pds::HashFold<9,x>
    #define BIT_10(x) pds::HashFold<10,x>
    #define BIT_11(x) pds::HashFold<11,x>
    #define BIT_12(x) pds::HashFold<12,x>
    #define BIT_13(x) pds::HashFold<13,x>
    #define BIT_14(x) pds::HashFold<14,x>
    #define BIT_15(x) pds::HashFold<15,x>
    #define BIT_16(x) pds::HashFold<16,x>

    //
    // Universal Hash
    //
    
   
    template <size_t M, size_t a, size_t b>
    struct universal
    {
        static_assert(a > 0, 
                      "universal: 'a' must be a positive integer");

        static_assert(a & 1,  
                      "universal: 'a' must be an odd integer");

        template <typename T>
        size_t operator()(T x) const              
        {
            static_assert(b < (1ULL << (sizeof(T)*8 - M)) , "universal: 'b' must be less than 2^(bitsize(T)-M)");
            size_t mask = (M >= sizeof(size_t)*8 ? (size_t)-1 : ((1ULL << M)-1));
            return ((a*x+b) >> (sizeof(T)*8-M)) & mask; 
        }
    };
    
    //
    // hash_rank
    //

    template <typename T>
    struct hash_rank
    {
        enum : size_t { value = 1 };
    };
    template <typename ...Hs>
    struct hash_rank<ModularHash<Hs...>>
    {
        enum : size_t { value = sizeof...(Hs) };
    };

    //
    // hash_bitsize
    //

    template <typename Hash>
    struct hash_bitsize
    {
        enum : size_t { value = sizeof(Hash{}(0)) * 8} ;
    };
    template <typename T>
    struct hash_bitsize<std::hash<T>>
    {
        enum : size_t { value = sizeof(size_t) * 8} ;
    };
    template <size_t N, typename Hash>
    struct hash_bitsize<HashFold<N, Hash>>
    {
        enum : size_t { value = N };
    };

    template <size_t M, size_t a, size_t b>
    struct hash_bitsize<universal<M, a, b>>
    {
        enum : size_t { value = M };
    };
    template <typename ...Hs>
    struct hash_bitsize<ModularHash<Hs...>>
    {
        enum : size_t { value = hash_total_bitsize<Hs...>::value };
    };

    // hash_total_bitsize...
    //

    template <typename ...Hs> struct hash_total_bitsize;
    
    template <> 
    struct hash_total_bitsize<>
    {
        enum : size_t { value = 0 };
    };
    template <typename H0, typename ...Hs> 
    struct hash_total_bitsize<H0, Hs...>
    {
        enum : size_t { value = hash_bitsize<H0>::value + hash_total_bitsize<Hs...>::value };
    };

    //
    // hash_offset
    //
    
    template <size_t N, typename H, typename ...Hs>
    struct hash_offset
    {
        enum : size_t { value = hash_bitsize<H>::value + hash_offset<N-1, Hs...>::value };
    };

    template <typename H, typename ...Hs>
    struct hash_offset<0, H, Hs...>
    {
        enum : size_t { value = 0 };
    };
    

    //
    // Few hash functions...
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

    struct H7
    {
        template <typename T>
        uint32_t operator()(T value) const
        {
            uint32_t x = static_cast<uint32_t>(value);
            return x ^ (x << 5)^ (x << 13) ^ (x << 20);
        }
    };

           
    struct Wang6
    {
     	uint32_t operator()(uint32_t a) const
		{
		    a = (a+0x7ed55d16) + (a<<12);
		    a = (a^0xc761c23c) ^ (a>>19);
		    a = (a+0x165667b1) + (a<<5);
		    a = (a+0xd3a2646c) ^ (a<<9);
		    a = (a+0xfd7046c5) + (a<<3);
		    a = (a^0xb55a4f09) ^ (a>>16);
		    return a;
		}
    };

    struct Wang7
    {
		uint32_t operator()(uint32_t a) const
		{
			a -= (a<<6);
			a ^= (a>>17);
			a -= (a<<9);
			a ^= (a<<4);
			a -= (a<<3);
			a ^= (a<<10);
			a ^= (a>>15);
			return a;
		}
    };


	struct WangHalfAvalanche
	{
		uint32_t operator()(uint32_t a) const 
		{ 
			a += ~(a<<15); 
			a ^= (a>>10); 
			a += (a<<3); 
			a ^= (a>>6); 
			a += ~(a<<11); 
			a ^= (a>>16); 
			return a;
		}
    };
	

	struct JavaIntHash
	{
		uint32_t operator()(uint32_t h)  const
		{ 
			h ^= (h >> 20) ^ (h >> 12); 
			return h ^ (h >> 7) ^ (h >> 4); 
		}
	};


	struct HalfAvalanche
	{
		uint32_t operator()(uint32_t a) const
		{
			a = (a+0x479ab41d) + (a<<8);
			a = (a^0xe4aa10ce) ^ (a>>5);
			a = (a+0x9942f0a6) - (a<<14);
			a = (a^0x5aedd67d) ^ (a>>3);
			a = (a+0x17bea992) + (a<<7);
			return a;
		}
    };

	struct HalfAvalanche2
	{
		uint32_t operator()(uint32_t a) const
		{
			a -= (a<<6);
			a ^= (a>>17);
			a -= (a<<9);
			a ^= (a<<4);
			a -= (a<<3);
			a ^= (a<<10);
			a ^= (a>>15);
			return a;
		}
    };

} // namespace pds




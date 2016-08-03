#include "pds/tuple.hpp"
#include "pds/hash.hpp"
#include "pds/range.hpp"
#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/cartesian.hpp"

#include <cat/show.hpp>
#include <cat/bits/type.hpp>

#include <iostream>

#include <yats.hpp>

using namespace yats;
using namespace pds;

auto g = Group("Complete")

    .Single("simple", []
    {
        pds::sketch< uint16_t
                   , (1<<20)
                   , pds::ModularHash<5, H1, H1, H1, H1> 
                   , pds::ModularHash<5, H2, H2, H2, H2> 
                   , pds::ModularHash<5, H3, H3, H3, H3> 
                   , pds::ModularHash<5, H4, H4, H4, H4> 
                   > s;

        for(int i = 0; i < 1000; i++)
        {
            s.increment_buckets(std::make_tuple(0xaaaa, 0xbbbb, 0xdead, 0xbeef));
            s.increment_buckets(std::make_tuple(0xdead, 0xbeef, 0xcafe, 0xbabe));
        }

        // 
        // get the index of the buckets whose value is greater than 1000...
        //

        auto idx = s.index_buckets([](auto &b)
                                 {
                                    return b > 500;
                                 });

        std::cout << "idx: " << cat::show(idx) << std::endl;

        auto r = pds::all_candidates(s, 
                                     std::make_tuple(pds::numeric_range<uint16_t>(0, 0xffff), 
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff)),
                                      idx
                                    );

        auto res = pds::expand_cartesian_product_by(r, pds::merge_annotated);

        std::cout << std::hex << std::endl;
        for(auto & t: res)
             std::cout << "candidate => " << t << std::endl;

    })

    .Single("advanced", []
    {
        pds::sketch< uint16_t
                   , (1<< 21)
                   , pds::ModularHash<3, H1, H1, H1, H1, H1, H1, H1> 
                   , pds::ModularHash<3, H2, H2, H2, H2, H2, H2, H2> 
                   , pds::ModularHash<3, H3, H3, H3, H3, H3, H3, H3> 
                   , pds::ModularHash<3, H4, H4, H4, H4, H4, H4, H4> 
                   , pds::ModularHash<3, H5, H5, H5, H5, H5, H5, H5> 
                   , pds::ModularHash<3, H6, H6, H6, H6, H6, H6, H6> 
                   > s;

        for(int i = 0; i < 1000; i++)
        {
            s.increment_buckets(std::make_tuple(0xbad,  0xbee,  0xdead, 0xbeef, 6010, 4216 , 6));
            s.increment_buckets(std::make_tuple(0xdead, 0xbeef, 0xcafe, 0xbabe, 80,   6667 , 17));
        }

        // 
        // get the index of the buckets whose value is greater than 1000...
        //

        auto idx = s.index_buckets([](auto &b)
                                 {
                                    return b > 500;
                                 });

        std::cout << "idx: " << cat::show(idx) << std::endl;
        
        auto r = pds::all_candidates(s, 
                                     std::make_tuple(pds::numeric_range<uint16_t>(0, 0xffff), 
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 65535),
                                                     pds::numeric_range<uint16_t>(0, 65535),
                                                     pds::numeric_range<uint16_t>(0, 255)
                                                     ),
                                      idx
                                    );


        // std::cout << std::dec << std::endl;

        // pds::tuple_foreach_index([](auto I, auto &elem) {
        //     std::cout << I << ":{ ";
        //     for(auto & x : elem)
        //         std::cout << x << " ";
        //     std::cout << "}" << std::endl;
        // }, r);

        auto res = pds::expand_cartesian_product_by(r, pds::merge_annotated);

        for(auto & t: res)
                std::cout << "candidate => " << t << std::endl;
    })

    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


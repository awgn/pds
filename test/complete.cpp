#include "pds/tuple.hpp"
#include "pds/hash.hpp"
#include "pds/range.hpp"
#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/cartesian.hpp"
#include "pds/hyperloglog.hpp"

// #include <cat/show.hpp>
// #include <cat/bits/type.hpp>

#include <iostream>
#include <random>

#include <yats.hpp>

using namespace yats;
using namespace pds;

auto g = Group("Complete")

    .Single("simple", []
    {
        pds::sketch< uint16_t
                   , (1<<20)
                   , pds::ModularHash< BIT_5(H1), BIT_5(H1), BIT_5(H1), BIT_5(H1)> 
                   , pds::ModularHash< BIT_5(H2), BIT_5(H2), BIT_5(H2), BIT_5(H2)> 
                   , pds::ModularHash< BIT_5(H3), BIT_5(H3), BIT_5(H3), BIT_5(H3)> 
                   , pds::ModularHash< BIT_5(H4), BIT_5(H4), BIT_5(H4), BIT_5(H4)> 
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

        // std::cout << "idx: " << cat::show(idx) << std::endl;

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
                   , pds::ModularHash<BIT_3(H1), BIT_3(H1), BIT_3(H1), BIT_3(H1), BIT_3(H1), BIT_3(H1), BIT_3(H1)> 
                   , pds::ModularHash<BIT_3(H2), BIT_3(H2), BIT_3(H2), BIT_3(H2), BIT_3(H2), BIT_3(H2), BIT_3(H2)> 
                   , pds::ModularHash<BIT_3(H3), BIT_3(H3), BIT_3(H3), BIT_3(H3), BIT_3(H3), BIT_3(H3), BIT_3(H3)> 
                   , pds::ModularHash<BIT_3(H4), BIT_3(H4), BIT_3(H4), BIT_3(H4), BIT_3(H4), BIT_3(H4), BIT_3(H4)> 
                   , pds::ModularHash<BIT_3(H5), BIT_3(H5), BIT_3(H5), BIT_3(H5), BIT_3(H5), BIT_3(H5), BIT_3(H5)> 
                   , pds::ModularHash<BIT_3(H6), BIT_3(H6), BIT_3(H6), BIT_3(H6), BIT_3(H6), BIT_3(H6), BIT_3(H6)> 
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

        // std::cout << "idx: " << cat::show(idx) << std::endl;
        
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
                std::cout << "candidate => " << t.value << std::endl;
    })

    .Single("hyperloglog-rsketch", []
    {
        using loglog_t = pds::hyperloglog                                                                               
                    <  uint8_t                               
                    ,  64
                    ,  std::hash<std::tuple<uint16_t, uint16_t>>         
                    >;                                                                                                                 
                     
        pds::sketch< loglog_t                      
                   , (1 << 16)
                   , pds::ModularHash<BIT_4(H1), BIT_4(H1), BIT_4(H1), BIT_4(H1)>   // IP components...
                   , pds::ModularHash<BIT_4(H2), BIT_4(H2), BIT_4(H2), BIT_4(H2)> 
                   , pds::ModularHash<BIT_4(H3), BIT_4(H3), BIT_4(H3), BIT_4(H3)> 
                   , pds::ModularHash<BIT_4(H4), BIT_4(H4), BIT_4(H4), BIT_4(H4)> 
                   , pds::ModularHash<BIT_4(H5), BIT_4(H5), BIT_4(H5), BIT_4(H5)> 
                   > s;

        std::mt19937 rand;

        for(int i = 0; i < 65000; i++)
        {
            auto sport = 20;
            auto dport = i;

            s.foreach_bucket(std::make_tuple(0xbad,  0xbee,  0xdead, 0xbeef), [&](auto &hllc) 
            {
                hllc(std::tuple<uint16_t, uint16_t>{sport, dport});
            });
        }

        // 
        // get the index of the buckets whose value is greater than 1000...
        //

        auto idx = s.index_buckets([](auto &b)
                                 {
                                    return b.cardinality() > 10000;
                                 });

        // std::cout << "idx: " << cat::show(idx) << std::endl;
        
        
        auto r = pds::all_candidates(s, 
                                     std::make_tuple(pds::numeric_range<uint16_t>(0, 0xffff), 
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff),
                                                     pds::numeric_range<uint16_t>(0, 0xffff)
                                                     ),
                                      idx
                                    );

        auto res = pds::expand_cartesian_product_by(r, pds::merge_annotated);

        for(auto & t: res)
            std::cout << "candidate => " << t.value << std::endl;

        s.foreach_index(idx, [](auto &hllc) 
        {
            std::cout << "cardinality => " << hllc.cardinality() << std::endl;
        });
    })

    .Single("final", []
    {
        using loglog_t = pds::hyperloglog                                                                               
                    <  uint8_t                               
                    ,  64
                    ,  std::hash<std::tuple<uint16_t, uint16_t>>         
                    >;                                                                                                                 
                     
        pds::sketch< loglog_t                      
                   , (1 << 16)
                   , pds::ModularHash<BIT_4(H1), BIT_4(H1), BIT_4(H1), BIT_4(H1)>   // IP components...
                   , pds::ModularHash<BIT_4(H2), BIT_4(H2), BIT_4(H2), BIT_4(H2)> 
                   , pds::ModularHash<BIT_4(H3), BIT_4(H3), BIT_4(H3), BIT_4(H3)> 
                   , pds::ModularHash<BIT_4(H4), BIT_4(H4), BIT_4(H4), BIT_4(H4)> 
                   , pds::ModularHash<BIT_4(H5), BIT_4(H5), BIT_4(H5), BIT_4(H5)> 
                   > s;

        std::mt19937 rand;

        for(int i = 0; i < 65000; i++)
        {
            auto sport = 20;
            auto dport = i;

            s.foreach_bucket(std::make_tuple(0xbad,  0xbee,  0xdead, 0xbeef), [&](auto &hllc) 
            {
                hllc(std::tuple<uint16_t, uint16_t>{sport, dport});
            });
        }

        // 
        // get the index of the buckets whose value is greater than 1000...
        //

        auto idx = s.index_buckets([](auto &b)
                                 {
                                    return b.cardinality() > 10000;
                                 });

        auto res = pds::reverse_sketch<uint16_t, uint16_t, uint16_t, uint16_t>(s, idx); 

        for(auto & t: res)
            std::cout << "candidate => " << t.value << std::endl;

        s.foreach_index(idx, [](auto &hllc) 
        {
            std::cout << "cardinality => " << hllc.cardinality() << std::endl;
        });
    })
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


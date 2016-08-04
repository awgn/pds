#include "pds/sketch.hpp"
#include "pds/range.hpp"

#include <iostream>
#include <stdexcept>
#include <limits>

#include <yats.hpp>

using namespace yats;
using namespace pds;

struct konst_hash
{
    konst_hash()
    { }

    size_t operator()(int) const
    {
        return 42;
    }
};


struct hash1
{
    uint64_t operator()(uint64_t x) const
    {
        return x & (x >> 16) & 0xffff;
    }
};


struct hash2
{
    uint64_t operator()(uint64_t x) const
    {
        return x & (x >> 13) & 0xffff;
    }
};


auto g = Group("Sketch")

    .Single("simple", []
    {
        pds::sketch<int, (1<<10), bit_10(std::hash<int>), bit_10(std::hash<int>) > sk;

        sk.foreach_bucket(10, [](int &ctr)
        {
            ctr = 1;
        });

        Assert( sk.count(10) == 1 );
    })


    .Single("hash", []
    {
        pds::sketch<int, (1 << 5), bit_5(konst_hash), bit_5(konst_hash)> sk;

        sk.foreach_bucket(1, [](int &ctr)
        {
            std::cout << (void *)&ctr << " = " << ctr << std::endl;
            ctr = 1;
        });

        Assert (sk.count(1), is_equal_to(1));

        sk.foreach_bucket(11, [](int &ctr)
        {
            std::cout << (void *)&ctr << " = " << ctr << std::endl;
            ctr = 2;
        });

        Assert (sk.count(1),  is_equal_to(2));
        Assert (sk.count(11), is_equal_to(2));
        Assert (sk.count(42), is_equal_to(2));
    })


    .Single("incr_decr", []
    {
        pds::sketch<int, 1024, HashFold<10, std::hash<int>> > sk;

        sk.increment_buckets(11);
        Assert(sk.count(11), is_equal_to(1));
        sk.increment_buckets(11);
        Assert(sk.count(11), is_equal_to(2));
        sk.increment_buckets(11);
        Assert(sk.count(11), is_equal_to(3));

        sk.increment_buckets(7);
        Assert(sk.count(7), is_equal_to(1));
        sk.increment_buckets(7);
        Assert(sk.count(7), is_equal_to(2));

        sk.decrement_buckets(7);
        Assert(sk.count(7), is_equal_to(1));

        Assert(sk.count(42), is_equal_to(0));
    })

    .Single("reset", []
    {
        pds::sketch<int, 1024, HashFold<10, std::hash<int>> > sk;

        sk.increment_buckets(11);
        sk.increment_buckets(11);
        sk.increment_buckets(11);
        Assert(sk.count(11), is_equal_to(3));

        sk.reset();
        Assert(sk.count(11), is_equal_to(0));
    })

    .Single("for_all", []
    {
        pds::sketch<int, 1024, HashFold<10, std::hash<int>> > sk;

        sk.forall([](int &n)
        {
            n = 42;
        });

        Assert(sk.count(1), is_equal_to(42));
        Assert(sk.count(2), is_equal_to(42));
        Assert(sk.count(3), is_equal_to(42));
        Assert(sk.count(42), is_equal_to(42));
        Assert(sk.count(0xdeadbeef), is_equal_to(42));
    })

    .Single("size", []
    {
        pds::sketch<int, 1024, HashFold<10, std::hash<int>> > s;
        Assert(s.size() == std::make_pair<size_t, size_t>(1, 1024));
    })

    .Single("merge", []
    {
        pds::sketch<uint32_t, 1024, HashFold<10, std::hash<int>> > s1, s2;

        s1.increment_buckets(1);
        s2.increment_buckets(1);
        s2.increment_buckets(2);

        auto s = s1 + s2;

        Assert(s.count(1), is_equal_to(2));
        Assert(s.count(2), is_equal_to(1));
        Assert(s.count(3), is_equal_to(0));
    })


    .Single("k_ary_estimate", []
    {
        pds::sketch<int32_t, 1024, HashFold<10, std::hash<int>> > s1;

        s1.increment_buckets(1);
        s1.increment_buckets(1);
        s1.increment_buckets(1);

        s1.increment_buckets(2);
        s1.increment_buckets(2);

        s1.increment_buckets(3);

        std::cout << "k-ary estimate: " << s1.estimate(1) << std::endl;
        std::cout << "k-ary estimate: " << s1.estimate(2) << std::endl;
        std::cout << "k-ary estimate: " << s1.estimate(3) << std::endl;
    })
    

    .Single("filter", []
    {
        pds::sketch<uint32_t, 1024, HashFold<10, std::hash<int>> > s;

        s.increment_buckets(1);

        s.increment_buckets(2);
        s.increment_buckets(2);

        s.increment_buckets(3);
        s.increment_buckets(3);
        s.increment_buckets(3);


        auto k1 = s.filter(pds::numeric_range<int>(0,10), [](uint32_t const &bucket) {
                                return bucket > 0;
                          });

        Assert ( k1 == std::vector<int>{1, 2, 3} );
        
        auto k2 = s.filter(pds::numeric_range<int>(0,10), [](uint32_t const &bucket) {
                                return bucket > 1;
                          });

        Assert ( k2 == std::vector<int>{2, 3} );
        
        auto k3 = s.filter(pds::numeric_range<int>(0,10), [](uint32_t const &bucket) {
                                return bucket > 2;
                          });

        Assert ( k3 == std::vector<int>{3} );

    })

    .Single("large_filter", []
    {
        pds::sketch<uint32_t, (1<<6), bit_6(std::hash<uint32_t>),
                                      bit_6(hash1),
                                      bit_6(hash2)> ips;

        ips.increment_buckets(0xff114200);
        ips.increment_buckets(0xffffff00);

        auto bs = ips.index_buckets([](uint32_t bucket) {
                                        return bucket != 0; 
                                    });

        std::cout << std::dec << std::endl;

        for(auto &v : bs) {
            std::cout << "| ";
            for(auto &e : v)
                std::cout << e << ' ';
            std::cout << std::endl;
        }

        std::cout << "..." << std::endl;

        auto fip = ips.filter(pds::numeric_range<uint32_t>(0,std::numeric_limits<uint32_t>::max()), [](uint32_t bucket) {
                                return bucket != 0;
                          });

        for(auto ip : fip)
            std::cout << std::hex << ip << std::endl;


    })
    .Single("compile-time error", []
    {
        // using Hash1 = pds::sketch<uint32_t, 1024, std::hash<uint32_t>, pds::ModularHash<64, std::hash<uint32_t>, std::hash<uint32_t>> >;
        // Hash1 x;

        // using Hash2 = pds::sketch<uint32_t, 1024, std::hash<uint32_t>, pds::FoldHash<32, std::hash<uint32_t>> >;
        // Hash2 x;
    })
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


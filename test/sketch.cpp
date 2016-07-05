#include "pds/sketch.hpp"

#include <iostream>
#include <stdexcept>

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


auto g = Group("Sketch")

    .Single("simple", []
    {
        pds::sketch<int, 1024, std::hash<int>, std::hash<int> > sk;

        sk.update_buckets(10, [](int &ctr)
        {
            ctr = 1;
        });

        Assert( sk.count(10) == 1 );
    })


    .Single("hash", []
    {
        pds::sketch<int, 1024, konst_hash, konst_hash> sk;

        sk.update_buckets(1, [](int &ctr)
        {
            std::cout << (void *)&ctr << " = " << ctr << std::endl;
            ctr = 1;
        });

        Assert (sk.count(1), is_equal_to(1));

        sk.update_buckets(11, [](int &ctr)
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
        pds::sketch<int, 1024, std::hash<int> > sk;

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
        pds::sketch<int, 1024, std::hash<int> > sk;

        sk.increment_buckets(11);
        sk.increment_buckets(11);
        sk.increment_buckets(11);
        Assert(sk.count(11), is_equal_to(3));

        sk.reset();
        Assert(sk.count(11), is_equal_to(0));
    })

    .Single("for_all", []
    {
        pds::sketch<int, 1024, std::hash<int> > sk;

        sk.for_all([](int &n)
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
        pds::sketch<int, 1024, std::hash<int> > s;
        Assert(s.size() == std::make_pair<size_t, size_t>(1, 1024));
    })

    .Single("merge", []
    {
        pds::sketch<uint32_t, 1024, std::hash<int> > s1, s2;

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
        pds::sketch<int32_t, 1024, std::hash<int> > s1;

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
    
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


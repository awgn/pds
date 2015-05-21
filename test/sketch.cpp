#include "pds/sketch.hpp"

#include <iostream>
#include <stdexcept>

#include <yats.hpp>

using namespace yats;


Context(Sketch)
{
    Test(simple)
    {
        pds::sketch<int, 1024, std::hash<int>, std::hash<int> > sk;

        sk.update_with(10, [](int &ctr)
        {
            ctr = 1;
        });

        Assert( sk.count(10) == 1 );
    }


    struct konst_hash
    {
        konst_hash()
        { }

        konst_hash(int n)
        : value_(n)
        {}

        size_t operator()(int) const
        {
            return value_;
        }

        size_t value_;
    };


    Test(hash)
    {
        pds::sketch<int, 1024, konst_hash, konst_hash> sk(konst_hash{42}, konst_hash{11});

        sk.update_with(1, [](int &ctr)
        {
            std::cout << (void *)&ctr << " = " << ctr << std::endl;
            ctr = 1;
        });

        Assert (sk.count(1), is_equal_to(1));

        sk.update_with(11, [](int &ctr)
        {
            std::cout << (void *)&ctr << " = " << ctr << std::endl;
            ctr = 2;
        });

        Assert (sk.count(1), is_equal_to(2));
        Assert (sk.count(11), is_equal_to(2));
        Assert (sk.count(42), is_equal_to(2));
    }


    Test(incr_decr)
    {
        pds::sketch<int, 1024, std::hash<int> > sk;

        sk.increment(11);
        Assert(sk.count(11), is_equal_to(1));
        sk.increment(11);
        Assert(sk.count(11), is_equal_to(2));
        sk.increment(11);
        Assert(sk.count(11), is_equal_to(3));

        sk.increment(7);
        Assert(sk.count(7), is_equal_to(1));
        sk.increment(7);
        Assert(sk.count(7), is_equal_to(2));

        sk.decrement(7);
        Assert(sk.count(7), is_equal_to(1));

        Assert(sk.count(42), is_equal_to(0));
    }

    Test(reset)
    {
        pds::sketch<int, 1024, std::hash<int> > sk;

        sk.increment(11);
        sk.increment(11);
        sk.increment(11);
        Assert(sk.count(11), is_equal_to(3));

        sk.reset();
        Assert(sk.count(11), is_equal_to(0));
    }

    Test(for_all)
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
    }
}


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


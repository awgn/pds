#include "pds/bloom.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;


Context(BloomFilter)
{
    Test(simple)
    {
        pds::bloom_filter<1024, std::hash<int>> bloom;

        bloom.set(11);
        bloom.set(42);

        Assert(bloom.is_set(42));
        Assert(bloom.is_set(11));
        Assert(!bloom.is_set(13));
    }

    Test(reset)
    {
        pds::bloom_filter<1024, std::hash<int>> bloom;

        bloom.set(42);
        bloom.reset();

        Assert(!bloom.is_set(42));
    }
}


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


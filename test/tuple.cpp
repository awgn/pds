#include "pds/tuple.hpp"
#include "pds/hash.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;

struct id
{
    template <typename T>
    T operator()(T x) const
    {
        return x;
    }
};


auto g = Group("Tuple")

    .Single("modular", []
    {
        pds::ModularHash<std::hash<int>> mod(std::hash<int>{});

        Assert(mod(std::make_tuple(0)) == 0);
        Assert(mod(std::make_tuple(42)) == 42);
        Assert(mod(std::make_tuple(256)) == 0);


        Assert(mod(std::make_tuple(0,0)) == 0);
        Assert(mod(std::make_tuple(0,1)) == 1);

        Assert(mod(std::make_tuple(1, 0)) == 256);
        Assert(mod(std::make_tuple(1, 1)) == 257);

    });



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


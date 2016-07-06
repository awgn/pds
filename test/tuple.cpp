#include "pds/tuple.hpp"

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

    .Single("hash_tuple", []
    {
        auto t = std::make_tuple(1,2);

        Assert( pds::hash_tuple(id{}, t, 2) == ((1<<2) | 2));
    });


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


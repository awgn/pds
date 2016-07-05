#include "pds/range.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;


auto g = Group("Range")

    .Single("simple", []
    {
        pds::numeric_range<int> r(1,3);

        Assert( r() == 1 );
        Assert( r() == 2 );
        Assert( r() == 3 );
        Assert( r() == pds::nullopt );
        Assert( r() == pds::nullopt );

    });


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


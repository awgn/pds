#include "pds/range.hpp"

#include <iostream>
#include <algorithm>

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

    })
    
    .Single("iterator", []
    {
        auto it  = pds::range_iterator<int>(1,3);
        auto end = pds::range_iterator<int>();
        for(; it != end; ++it)
            std::cout << *it << std::endl;
    })

    .Single("for-range", []
    {
        for(auto e : pds::numeric_range<int>(1,3))
        {
            std::cout << e << std::endl;
        }
    })

    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


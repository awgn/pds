#include "pds/range.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

#include <yats.hpp>

using namespace yats;


struct Test { };

namespace pds
{
    template <> struct make_range<Test>
    {
        static 
        std::vector<int> run()
        {
            return std::vector<int>{1,2,3};
        }
    };
}


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

    .Single("make_range", []
    {
        for(auto e : pds::make_range<uint8_t>::run())
        {
            std::cout << (int)e << std::endl;
        }
    })

    .Single("user-defined-range", []
    {
        for(auto e : pds::make_range<Test>::run())
        {
            std::cout << (int)e << std::endl;
        }
    })
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


#include "pds/hash.hpp"
#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"

#include <iostream>
#include <random>

#include <yats.hpp>

using namespace yats;


auto g = Group("LogLog")

    .Single("simple", []
    {
        pds::loglog<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })

    .Single("merge", []
    {
        pds::loglog<std::string> llc;
        llc += llc;
    })
    ;


auto h = Group("Hyper")

    .Single("simple", []
    {
        pds::hyperloglog<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })

    .Single("merge", []
    {
        pds::hyperloglog<std::string> llc;
        llc += llc;

        Assert( llc.cardinality(), is_equal_to(0));
    })
    
    .Single("hashing", []
    {
        pds::hyperloglog<int, 16, pds::H2> llc;

        std::mt19937 rand;

        for(int n = 0; n < 1000; n++)
        {
            llc(rand());
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })
    ;

int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


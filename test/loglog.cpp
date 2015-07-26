#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;


auto g = Group("LogLog")

    .Single("simple", []
    {
        std::cout << "LogLog: " << std::endl;

        pds::loglog<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << n << ": -> " << llc.cardinality() << std::endl;
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
        std::cout << "HyperLogLog: " << std::endl;

        pds::hyperloglog<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << n << ": -> " << llc.cardinality() << std::endl;
        }
    })

    .Single("merge", []
    {
        pds::hyperloglog<std::string> llc;
        llc += llc;

        Assert( llc.cardinality(), is_equal_to(0));
    })
    ;

int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


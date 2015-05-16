#include "stream/loglog.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;


Context(LogLog)
{
    Test(simple)
    {
        std::cout << "LogLog: " << std::endl;

        stream::loglog::counter<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << n << ": -> " << llc.cardinality() << std::endl;
        }
    }

    Test(merge)
    {
        stream::loglog::counter<std::string> llc;
        llc += llc;
    }
}


Context(Hyper)
{
    Test(simple)
    {
        std::cout << "HyperLogLog: " << std::endl;

        stream::hyperloglog::counter<std::string> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << n << ": -> " << llc.cardinality() << std::endl;
        }
    }

    Test(merge)
    {
        stream::hyperloglog::counter<std::string> llc;
        llc += llc;

        Assert( llc.cardinality(), is_equal_to(0));
    }
}


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


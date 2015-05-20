#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"

#include <iostream>
#include <random>

int
main(int argc, char *argv[])
{
    pds::loglog::counter<int, 1024> c;
    pds::hyperloglog::counter<int, 1024> h;

    std::mt19937 rand;

    for(int x = 0; x < 20000; x++)
    {
        auto hash = rand();

        c(hash);
        h(hash);

        std::cout << x << ' ' << c.cardinality() << ' ' << h.cardinality() << std::endl;
    }

    return 0;
}


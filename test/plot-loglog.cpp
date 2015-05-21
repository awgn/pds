#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"

#include <iostream>
#include <random>

int
main(int, char *[])
{
    pds::loglog<int, 1024> c;
    pds::hyperloglog<int, 1024> h;

    std::mt19937 rand;

    for(int x = 0; x < 100000; x++)
    {
        auto hash = rand();

        c(hash);
        h(hash);

        std::cout << x << ' ' << c.cardinality() << ' ' << h.cardinality() << std::endl;
    }

    return 0;
}


#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"

#include <iostream>
#include <random>

int
main(int, char *[])
{
    pds::loglog<uint8_t, 1024, std::hash<int>> c;
    pds::hyperloglog<uint8_t, 1024, std::hash<int>> h;

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


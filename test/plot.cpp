#include "stream/loglog.hpp"

#include <iostream>
#include <random>

int
main(int argc, char *argv[])
{
    stream::loglog::counter<int, 1<<10> c;
    stream::hyperloglog::counter<int, 1<<10> h;

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


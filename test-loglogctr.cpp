#include "loglogctr.hpp"

#include <iostream>

int
main(int argc, char *argv[])
{
    prob::LogLogCounter<int> ctr;

    for(int n = 0; n < 1024; n++)
    {
        ctr(n);
        std::cout << ctr.value() << std::endl;
    }

    return 0;
}


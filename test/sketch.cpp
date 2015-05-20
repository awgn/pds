#include "pds/sketch.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;


Context(Sketch)
{
    Test(simple)
    {
        pds::sketch<int, 1024, std::hash<int>, std::hash<int> > sk;

        sk(10, [](int &value)
          {
                std::cout << (void *) &value << " = " << value << std::endl;
                value = 10;
          });

        sk(10, [](int &value)
          {
                std::cout << (void *) &value << " = " << value << std::endl;
                value = 10;
          });

    }
}


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


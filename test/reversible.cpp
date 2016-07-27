#include "pds/sketch.hpp"
#include "pds/reversible.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;

struct id
{
    template <typename T>
    T operator()(T x) const
    {
        return x;
    }
};


auto g = Group("Reversible")

    .Single("sketch", []
    {
        pds::sketch< int
                   , 65536
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> 
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> > s;

    })
    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


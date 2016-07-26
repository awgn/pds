#include "pds/tuple.hpp"
#include "pds/modular.hpp"

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


auto g = Group("Tuple")

    .Single("modular", []
    {
    });


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


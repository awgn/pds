#include "pds/cartesian.hpp"

#include <string>
#include <iostream>

#include <yats.hpp>

using namespace yats;

auto g = Group("Cartesian")

    .Single("product", []
    {
        auto p = std::make_tuple(std::vector<int>{1,2,3},
                                 std::vector<std::string>{"hello", "world"});

        auto r = pds::cartesian_product(p);

        for(auto &x : r)
        {
            std::cout << std::get<0>(x) << ' ' << std::get<1>(x) << std::endl;
        }

    });

int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


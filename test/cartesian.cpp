#include "pds/cartesian.hpp"
#include "pds/tuple.hpp"

#include <string>
#include <iostream>

#include <yats.hpp>

using namespace yats;

auto g = Group("Cartesian")

    .Single("simple", []
    {
        auto p = std::make_tuple(std::vector<int>{1,2,3},
                                 std::vector<std::string>{"hello", "world"});

        auto r = pds::cartesian_product(p);
             
        for(auto &x : r)
        {
            std::cout << x << std::endl;
            // std::cout << std::get<0>(x) << ' ' << std::get<1>(x) << std::endl;
        }

    })
    .Single("unpacked", []
    {
        auto p = std::make_tuple(std::vector<int>{1,2,3},
                                 std::vector<std::string>{"hello", "world"});

        auto r = pds::unpacked_cartesian_product(p);
             
        auto p2 = std::make_tuple(r, std::vector<char>{'a', 'b'});

        auto r2 = pds::unpacked_cartesian_product(p2);

        for(auto &x : r2)
        {
             std::cout << x << std::endl;
        }

    });




int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


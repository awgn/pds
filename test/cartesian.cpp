#include "pds/cartesian.hpp"
#include "pds/tuple.hpp"
#include "pds/annotated.hpp"
#include "pds/reversible.hpp"

#include <string>
#include <iostream>

#include <yats.hpp>

using namespace yats;
using namespace pds;

auto g = Group("Cartesian")

    .Single("cartesian_product", []
    {
        std::vector<int>  v {1, 2, 3};
        std::vector<char> c {'a','b'};
        auto p = pds::cartesian_product(v,c);
        for(auto & e : p)
            std::cout << e << std::endl;
    })

    .Single("cartesian_product_by", []
    {
        std::vector<pds::annotated<int, Indices>> v1 = {
                                             pds::annotate(150, Indices { std::vector<size_t>{2,5}, 
                                                                                std::vector<size_t>{1}, }),
                                             pds::annotate(47,  Indices { std::vector<size_t>{3}, 
                                                                                std::vector<size_t>{5}, }),
                                             pds::annotate(236, Indices { std::vector<size_t>{2}, 
                                                                                std::vector<size_t>{2,3,7} })
                                          };


        std::vector<pds::annotated<int, Indices>> v2 = {
                                             pds::annotate(72, Indices { std::vector<size_t>{1,2}, 
                                                                               std::vector<size_t>{1,5}, }),
                                             pds::annotate(104, Indices { std::vector<size_t>{1,2}, 
                                                                                std::vector<size_t>{2,6}, }),
                                          };

        auto p = pds::cartesian_product_by(v1,v2, pds::merge_annotated); 

        for(auto & e : p)
            std::cout << e << std::endl;

    })

    .Single("cartesian+merge", []
    {
        std::vector<pds::annotated<int, Indices>> v1 = {
                                             pds::annotate(150, Indices { std::vector<size_t>{2,5}, 
                                                                                std::vector<size_t>{1}, }),
                                             pds::annotate(47,  Indices { std::vector<size_t>{3}, 
                                                                                std::vector<size_t>{5}, }),
                                             pds::annotate(236, Indices { std::vector<size_t>{2}, 
                                                                                std::vector<size_t>{2,3,7} })
                                          };


        std::vector<pds::annotated<int, Indices>> v2 = {
                                             pds::annotate(72, Indices { std::vector<size_t>{1,2}, 
                                                                               std::vector<size_t>{1,5}, }),
                                             pds::annotate(104, Indices { std::vector<size_t>{1,2}, 
                                                                                std::vector<size_t>{2,6}, }),
                                          };

        auto p = pds::cartesian_product_by(v1,v2, pds::merge_annotated);

        std::vector<pds::annotated<int, Indices>> v3 = {
                                             pds::annotate(182, Indices{ std::vector<size_t>{1,2 }, 
                                                                               std::vector<size_t>{1}, }),
                                             pds::annotate(32,  Indices { std::vector<size_t>{2}, 
                                                                                std::vector<size_t>{1}, }),
                                             pds::annotate(49,  Indices { std::vector<size_t>{2}, 
                                                                                std::vector<size_t>{2, 6}, }),
                                          };

        auto p2 = pds::cartesian_product_by(p,v3, pds::merge_annotated);
 
        for(auto & e : p2)
            std::cout << e << std::endl;

    })

    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


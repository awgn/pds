#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/range.hpp"
#include "pds/cartesian.hpp"

#include <cat/show.hpp>

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

    .Single("sketch_basic", []
    {
        pds::sketch< int
                   , 65536
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> 
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> > s;


        auto r0 = pds::candidates<0>(s, pds::numeric_range<int>(0, 255), 
                                    std::vector<std::vector<size_t>>{ 
                                          std::vector<size_t>{1, 8, 42, 30023}
                                        , std::vector<size_t>{1, 8, 42, 30023}
                                    });
        
        auto r1 = pds::candidates<1>(s, pds::numeric_range<int>(0, 255), 
                                    std::vector<std::vector<size_t>>{ 
                                          std::vector<size_t>{1, 8, 42, 30023}
                                        , std::vector<size_t>{1, 8, 42, 30023}
                                    });

        for(auto const &x : r0)
        {
            std::cout << "r0: " << x << std::endl;
        }

        for(auto const &x : r1)
        {
            std::cout << "r1: " << x << std::endl;
        }

        auto res = pds::cartesian_product(r0, r1); 

        for(auto & t: res)
            std::cout << "candidate => " << t << std::endl;

    })
    .Single("sketch_all_candidates", []
    {
        pds::sketch< int
                   , 65536
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> 
                   , pds::ModularHash<8, std::hash<int>, std::hash<int>> > s;

        auto r = pds::all_candidates(s, 
                                     std::make_tuple(pds::numeric_range<int>(0, 255), 
                                                     pds::numeric_range<int>(0, 255)),

                                    std::vector<std::vector<size_t>>{ 
                                          std::vector<size_t>{1, 8, 42, 30023}
                                        , std::vector<size_t>{1, 8, 42, 30023}
                                    });

        std::cout << "R: " << std::endl;

        pds::tuple_foreach([](auto &elem) {
            for(auto & x : elem)
                std::cout << x << ' ';
            std::cout << std::endl;
        }, r);

        auto res = pds::expand_cartesian_product( r );

        for(auto & t: res)
             std::cout << "candidate => " << t << std::endl;
    })
    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


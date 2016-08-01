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

    .Single("merge_candidates", []
    {
        auto c1 = pds::make_candidate(150, { std::vector<size_t>{2,5}, 
                                             std::vector<size_t>{1},
                                             std::vector<size_t>{1,4,9},
                                             std::vector<size_t>{1,3,6},
                                             std::vector<size_t>{3} });

        auto c2 = pds::make_candidate(72, { std::vector<size_t>{1, 2}, 
                                            std::vector<size_t>{1, 5},
                                            std::vector<size_t>{4,9},
                                            std::vector<size_t>{5},
                                            std::vector<size_t>{3,7,8} });


        auto m = pds::merge_candidates(c1, c2, 1);
        if (m)
            std::cout << *m << std::endl;
        else
            std::cout << "nullopt!" << std::endl;
    })


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

        auto res = pds::cartesian_product( 
                        std::make_tuple(r0, r1)
                   );

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

        auto res = pds::cartesian_product( r );

        for(auto & t: res)
            std::cout << "candidate => " << t << std::endl;
    })
    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


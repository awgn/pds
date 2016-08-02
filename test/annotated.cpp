#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/range.hpp"
#include "pds/cartesian.hpp"

#include <cat/show.hpp>

#include <iostream>

#include <yats.hpp>

using namespace yats;
using namespace pds;

struct id
{
    template <typename T>
    T operator()(T x) const
    {
        return x;
    }
};


auto g = Group("Reversible")

    .Single("merge_annotated", []
    {
        auto c1 = pds::annotate(150, Indices{ std::vector<size_t>{2,5}, 
                                                    std::vector<size_t>{1},
                                                    std::vector<size_t>{1,4,9},
                                                    std::vector<size_t>{1,3,6},
                                                    std::vector<size_t>{3} });

        auto c2 = pds::annotate(72, Indices { std::vector<size_t>{1, 2}, 
                                                    std::vector<size_t>{1, 5},
                                                    std::vector<size_t>{4,9},
                                                    std::vector<size_t>{5},
                                                    std::vector<size_t>{3,7,8} });


        auto m = pds::merge_annotated(c1, c2, 1);
        if (m)
            std::cout << *m << std::endl;
        else
            std::cout << "nullopt!" << std::endl;
    })

    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


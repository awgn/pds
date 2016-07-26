#include "pds/tuple.hpp"
#include "pds/hash.hpp"

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
        pds::ModularHash<8, std::hash<int>> mod(std::hash<int>{});

        Assert(mod(std::make_tuple(0)) == 0);
        Assert(mod(std::make_tuple(42)) == 42);
        Assert(mod(std::make_tuple(256)) == 0);


    })
    .Single("modular2", []
    {
        pds::ModularHash<8, std::hash<int>,
                            std::hash<int>> mod2(std::hash<int>{});

        Assert(mod2(std::make_tuple(0,0)) == 0);
        Assert(mod2(std::make_tuple(0,1)) == 1);

        Assert(mod2(std::make_tuple(1, 0)) == 256);
        Assert(mod2(std::make_tuple(1, 1)) == 257);

    })

    .Single("foreach", []
    {
        auto t = std::make_tuple<int, int, int>(1,2,3);

        pds::tuple_foreach([](auto &elem) {

            std::cout << elem << ' ';

        }, t);

        std::cout << std::endl;
    })
    
    .Single("continue", []
    {
        auto t = std::make_tuple<int, int, int>(1,2,3);

        size_t n = 0;
        pds::tuple_continue([&](auto &elem) 
        {
            std::cout << elem << ' ';
            return ++n < 2;

        }, t);

        std::cout << std::endl;
    })

    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


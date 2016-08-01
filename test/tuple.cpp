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
 
    .Single("tuple_tail", []
    {
        auto t1 = std::make_tuple(0);
        auto t2 = std::make_tuple(0, 'a');
        auto t3 = std::make_tuple(0, 'a', 1.2);

        std::cout << pds::tail(t1) << std::endl; 
        std::cout << pds::tail(t2) << std::endl; 
        std::cout << pds::tail(t3) << std::endl; 
    })
   
    .Single("make_flat_tuple", []
    {
        auto t0 = std::make_tuple(0);
        auto t1 = std::make_tuple(0,1);
        auto t2 = std::make_tuple(std::make_tuple(0), 9);
        auto t3 = std::make_tuple(std::make_tuple(0), 1, 9);
        auto t4 = std::make_tuple(0, std::make_tuple(1), 9);
        auto t5 = std::make_tuple(std::make_tuple(0), std::make_tuple(1), 9);

        std::cout << pds::make_flat_tuple(t0) << std::endl; 
        std::cout << pds::make_flat_tuple(t1) << std::endl; 
        std::cout << pds::make_flat_tuple(t2) << std::endl; 
        std::cout << pds::make_flat_tuple(t3) << std::endl; 
        std::cout << pds::make_flat_tuple(t4) << std::endl; 
        std::cout << pds::make_flat_tuple(t5) << std::endl; 

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


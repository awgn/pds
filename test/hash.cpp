#include "pds/tuple.hpp"
#include "pds/hash.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;
using namespace pds;

auto g = Group("Hash")

    .Single("codomain-size", []
    {
        using Hash8  = pds::ModularHash<HashFold<8,std::hash<int>>>;
        using Hash16 = pds::ModularHash<HashFold<8,std::hash<int>>, HashFold<8,std::hash<int>>>;
        using Hash5  = pds::HashFold<5, std::hash<int>>;

        Assert(pds::hash_bitsize<std::hash<char>>::value, is_equal_to(64));
        Assert(pds::hash_bitsize<std::hash<int>>::value, is_equal_to(64));
        Assert(pds::hash_bitsize<Hash5>::value, is_equal_to(5));
        Assert(pds::hash_bitsize<Hash8>::value, is_equal_to(8));
        Assert(pds::hash_bitsize<Hash16>::value, is_equal_to(16));

    })
    
    .Single("rank", []
    {
        using Hash8  = pds::ModularHash<std::hash<int>>;
        using Hash16 = pds::ModularHash<std::hash<int>, std::hash<int>>;
        using Hash5  = pds::HashFold<5, std::hash<int>>;

        Assert(pds::hash_rank<std::hash<char>>::value, is_equal_to(1));
        Assert(pds::hash_rank<std::hash<int>>::value , is_equal_to(1));
        Assert(pds::hash_rank<Hash5>::value          , is_equal_to(1));
        Assert(pds::hash_rank<Hash8>::value          , is_equal_to(1));
        Assert(pds::hash_rank<Hash16>::value         , is_equal_to(2));

    })

    ;



int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}


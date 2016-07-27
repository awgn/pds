#include "pds/tuple.hpp"
#include "pds/hash.hpp"

#include <iostream>

#include <yats.hpp>

using namespace yats;

auto g = Group("Hash")

    .Single("codomain-size", []
    {
        using Hash8  = pds::ModularHash<8, std::hash<int>>;
        using Hash16 = pds::ModularHash<8, std::hash<int>, std::hash<int>>;
        using Hash5  = pds::FoldHash<5, std::hash<int>>;

        Assert(pds::hash_codomain_size<std::hash<char>>::value, is_equal_to(64));
        Assert(pds::hash_codomain_size<std::hash<int>>::value, is_equal_to(64));
        Assert(pds::hash_codomain_size<Hash5>::value, is_equal_to(5));
        Assert(pds::hash_codomain_size<Hash8>::value, is_equal_to(8));
        Assert(pds::hash_codomain_size<Hash16>::value, is_equal_to(16));

    })
    
    .Single("rank", []
    {
        using Hash8  = pds::ModularHash<8, std::hash<int>>;
        using Hash16 = pds::ModularHash<8, std::hash<int>, std::hash<int>>;
        using Hash5  = pds::FoldHash<5, std::hash<int>>;

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


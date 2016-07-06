#include <cstdint>
#include <tuple>
#include <vector>
#include <stdexcept>

namespace pds {

    //
    // implementation details...
    //

    namespace details
    {
        template <typename Fun, typename TupleT, size_t ...N>
        void tuple_foreach(Fun fun, TupleT &&tup, std::index_sequence<N...>)
        {
            std::initializer_list<bool> sink {(fun(std::get<N>(std::forward<TupleT>(tup))), true)...};
            (void)sink;
        }
    }
    
    //
    // index_tuple: create indexes for a tuple
    //

    template <typename TupleT>
    using index_tuple = std::make_index_sequence<std::tuple_size<std::decay_t<TupleT>>::value>;

    //
    // tuple_foreach...
    //
    
    template <typename Fun, typename TupleT>
    void tuple_foreach(Fun fun, TupleT &&tup)
    {
        return details::tuple_foreach(fun, std::forward<TupleT>(tup), index_tuple<TupleT>{});
    }

    //
    // hash_tuple...
    //
    
    template <typename Hash, typename ...Ts>
    uint64_t 
    hash_tuple(Hash fun, std::tuple<Ts...> const &tup, int bits)
    {
        uint64_t h = 0;
 
        if ((bits * sizeof...(Ts)) > sizeof(h)*8)
            throw std::runtime_error("hash_fold: too many hash values or hash size too large");
 
        pds::tuple_foreach([&](auto &elem) {
                h <<= bits;
                h |= (fun(elem) & ((1 << bits)-1));
        }, tup);

        return h;
    }

} // namespace pds



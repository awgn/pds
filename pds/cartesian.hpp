#include "pds/tuple.hpp"

#include <vector>
#include <algorithm>

namespace pds {

    namespace details
    {
        inline bool
        null_idx(std::vector<size_t> const &idx)
        {
            return std::all_of(std::begin(idx), std::end(idx), [](size_t n) { return n == 0; });
        }


        inline void
        next_idx(std::vector<size_t> const &lim, 
                 std::vector<size_t> &idx)
        {
            for(size_t i = 0; i < idx.size(); i++)
            {
                if (idx[i] < (lim[i]-1))
                {
                    idx[i]++;
                    return;
                }
                else 
                {
                    idx[i] = 0;
                }
            }
        }

        template <typename Fun>
        inline void
        run_idx(std::vector<size_t> const &lim, Fun fun)
        {
            std::vector<size_t> idx(lim.size(), 0);    

            do
            {
                fun(idx);
                next_idx(lim, idx);
            }
            while (!null_idx(idx));
        }


        template <typename ...Ts>
        inline auto 
        limits(std::tuple<std::vector<Ts>...> const &tup)
        {
            std::vector<size_t> ret;

            pds::tuple_foreach_index([&](auto N, auto const &v)
            {
                (void)N;
                ret.push_back(v.size());

            }, tup);

            return ret;
        }

    } // namespace details


    template <typename ...Ts>
    std::vector<std::tuple<Ts...>>
    cartesian_product(std::tuple<std::vector<Ts>...> const &in)
    {
        std::vector<std::tuple<Ts...>> ret;

        auto lim = details::limits(in);

        details::run_idx(lim, [&](std::vector<size_t> &idx) {

                std::tuple<Ts...> elem;

                tuple_foreach_index([&](auto Idx, auto &elem_i) { 

                    size_t constexpr const I = decltype(Idx)::value;
                    elem_i = std::get<I>(in)[idx[I]];

                }, elem);

                ret.push_back(std::move(elem));
        });

        return ret;
    }

} // pds

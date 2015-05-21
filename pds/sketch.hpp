#pragma once

#include <cstddef>
#include <utility>
#include <vector>
#include <limits>
#include <tuple>

#include "pds/utils.hpp"

namespace pds {

    template < typename T, std::size_t W, typename ...Fs>
    struct sketch
    {
        template <typename ...Hs>
        sketch(Hs && ...hs)
        : data_(sizeof...(Fs), std::vector<T>(W))
        , fs_(make_tuple<Fs...>(std::forward<Hs>(hs)...))
        {
        }

        template <typename Tp, typename Fun>
        void with(Tp const &data, Fun action)
        {
            apply(data, action, std::make_index_sequence<sizeof...(Fs)>());
        }

        template <typename Tp, typename Fun>
        void with(Tp const &data, Fun action) const
        {
            apply(data, action, std::make_index_sequence<sizeof...(Fs)>());
        }

        //
        // increment counters
        //

        template <typename Tp>
        void increment(Tp const &data)
        {
            with(data, [](T &ctr) { ++ctr; });
        }

        //
        // decrement counters
        //

        template <typename Tp>
        void decrement(Tp const &data)
        {
            with(data, [](T &ctr) { --ctr; });
        }

        //
        // min-count
        //

        template <typename Tp>
        T count(Tp const &data) const
        {
            T n = std::numeric_limits<T>::max();

            with(data, [&](T const &ctr) {
                n = std::min(n, ctr);
            });

            return n;
        }

        //
        // reset all counters in the sketch
        //

        void
        reset()
        {
            for(auto & v : data_)
                for(auto & e : v)
                    e = T{};
        }

        template <typename Fun>
        void
        reset_with(Fun f)
        {
            for(auto & v : data_)
                for(auto & e : v)
                    f(e);
        }

    private:

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>)
        {
            auto sink { (action(data_.at(N).at(std::get<N>(fs_)(data) % W)),0)... };
            (void)sink;
        }

        template <typename Tp, typename Fun, size_t ...N>
        void apply(Tp const &data, Fun action, std::index_sequence<N...>) const
        {
            auto sink { (action(data_.at(N).at(std::get<N>(fs_)(data) % W)),0)... };
            (void)sink;
        }

        std::vector<std::vector<T>> data_;
        std::tuple<Fs...> fs_;
    };

} // namespace pds

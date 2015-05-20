#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace pds {

    template < typename Tp, std::size_t W, typename ...Fs>
    struct sketch
    {
        sketch()
        : data_(sizeof...(Fs), std::vector<Tp>(W))
        { }

        template <typename T, typename Fun>
        void operator()(T const &data, Fun action)
        {
            apply(data, action, std::make_index_sequence<sizeof...(Fs)>());
        }

    private:

        template <typename T, typename Fun, size_t ...N>
        void apply(T const &data, Fun action, std::index_sequence<N...>)
        {
            auto sink { (action(data_.at(N).at(Fs{}(data) % W)),0)... };
            (void)sink;
        }

        std::vector<std::vector<Tp>> data_;
    };

} // namespace pds

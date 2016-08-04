#include <type_traits>

namespace pds {
    
    //
    // type_at: get N-type from a pack
    //

    template <size_t N, typename ...Ts> struct type_at;

    template <typename T, typename ...Ts>
    struct type_at<0, T, Ts...>
    {
        using type = T;
    };

    template <size_t N, typename T, typename ...Ts>
    struct type_at<N, T, Ts...> : type_at<N-1, Ts...>
    { };

    template <size_t N, typename ...Ts>
    using type_at_t = typename type_at<N, Ts...>::type;


} // namespace pds

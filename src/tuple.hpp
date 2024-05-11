#ifndef TUPLE_HPP
#define TUPLE_HPP

#ifndef NO_STD_TUPLE
    #include <tuple>
template<typename... Args>
using tuple = std::tuple<Args...>;
#else
    #include <tuplet/tuple.hpp>
template<typename... Args>
using tuple = tuplet::tuple<Args...>;
#endif

template<class Tuple, std::size_t N = 0>
constexpr auto get_at(Tuple& tup, size_t idx) -> decltype(auto) {
    [[assume(N < std::tuple_size_v<Tuple>)]];
    [[assume(idx < std::tuple_size_v<Tuple>)]];
#ifndef NO_STD_TUPLE
    if (N == idx) { return std::get<N>(tup); }
#else
    if (N == idx) { return tuplet::get<N>(tup); }
#endif

    if constexpr (N + 1 < std::tuple_size_v<Tuple>) { return get_at<Tuple, N + 1>(tup, idx); }

    std::unreachable();
}

#endif

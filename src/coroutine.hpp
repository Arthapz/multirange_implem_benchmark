#ifndef COROUTINE_HPP
#define COROUTINE_HPP

#include <utility>

#include "numerics_range.hpp"
#include "tuple.hpp"

#ifndef __clang__
    #include <generator>
#else
    #include "generator.hpp"
#endif

template<IsNumericsRange... Ranges>
struct LooperGenerator {
    using Type = tuple<NumericsRangeType<Ranges>...>;
};

template<IsNumericsRange... Ranges>
using LooperGeneratorType = LooperGenerator<Ranges...>::Type;

template<std::size_t N, typename Tuples>
constexpr auto next(const Tuples& starts,
                    const Tuples& bounds,
                    const Tuples& strides,
                    Tuples&       values) noexcept -> bool {
    for (auto i = 0u; i != N; ++i) {
        const auto  id            = N - 1 - i;
        auto&       current_value = get_at(values, id);
        const auto& stride        = get_at(strides, id);
        const auto& bound         = get_at(bounds, id);

        if ((current_value + stride) >= bound) [[unlikely]] {
            const auto& start = get_at(starts, id);
            current_value     = start;
        } else {
            current_value += stride;
            return true;
        }
    }
    return false;
}

template<IsNumericsRange... Ranges>
auto rangeImplementation(Ranges... ranges) noexcept
    -> std::generator<LooperGeneratorType<std::remove_cvref_t<Ranges>...>> {
    using TupleType = LooperGeneratorType<std::remove_cvref_t<Ranges>...>;

    static_assert(sizeof...(Ranges) > 1,
                  "Use range() instead of multiRange for 1 sized range generator");
    const auto starts  = TupleType { std::move(ranges.start)... };
    const auto bounds  = TupleType { std::move(ranges.bound)... };
    const auto strides = TupleType { std::move(ranges.stride)... };

    auto values = starts;
    do {
        co_yield values;
    } while (next<sizeof...(Ranges), TupleType>(starts, bounds, strides, values));
}

namespace coroutine {
    template<typename... Args>
    constexpr auto multiRange(Args&&... args) noexcept {
        return rangeImplementation(
            NumericsRange<std::remove_cvref_t<Args>> { .start  = 0,
                                                       .bound  = std::forward<Args>(args),
                                                       .stride = 1 }...);
    }
} // namespace coroutine

#endif

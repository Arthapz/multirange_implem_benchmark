#ifndef RANGES_HPP
#define RANGES_HPP

#include "numerics_range.hpp"

#include <cmath>
#include <ranges>
#include <type_traits>

template<IsNumericsRange Range>
constexpr auto computeSizes(const Range& ranges) noexcept -> std::size_t {
    return std::abs((ranges.bound - ranges.start) / ranges.stride);
}

template<IsNumericsRange... Ranges>
    requires(sizeof...(Ranges) > 1)
class MultiNumericsRangeView
    : public std::ranges::view_interface<MultiNumericsRangeView<Ranges...>> {
  public:
    using TupleType = LooperGeneratorType<std::remove_cvref_t<Ranges>...>;

  private:
    struct Iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type        = TupleType;
        using difference_type   = std::ptrdiff_t;
        using reference         = const value_type&;

        Iterator()
            requires(std::default_initializable<NumericsRangeType<Ranges>> and ...)
        = default;

        template<typename T>
        constexpr Iterator(T&& _starts, T&& _bounds, T&& _strides) noexcept
            : starts { std::forward<T>(_starts) }, bounds { std::forward<T>(_bounds) },
              strides { std::forward<T>(_strides) }, values { starts } {}

        constexpr auto operator==(const Iterator& other) const noexcept -> bool {
            return operator*() == *other;
        }

        constexpr auto operator*() const noexcept -> reference { return values; }

        constexpr auto operator++() noexcept -> Iterator& {
            static constexpr auto N = std::size_t { sizeof...(Ranges) };

            for (auto i = std::size_t { 0 }; i != N; ++i) {
                auto        id            = N - 1 - i;
                auto&       current_value = get_at(values, id);
                const auto& stride        = get_at(strides, id);
                const auto& bound         = get_at(bounds, id);

                if ((current_value + stride) >= bound) [[unlikely]] {
                    const auto& start = get_at(starts, id);
                    current_value     = start;
                } else {
                    current_value += stride;
                    return *this;
                }
            }
            finished = true;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> void { ++*this; }

        TupleType starts;
        TupleType bounds;
        TupleType strides;
        TupleType values;
        bool      finished = false;
    };

    struct Sentinel {
        friend constexpr auto operator==(const Iterator& it, const Sentinel&) noexcept -> bool {
            return it.finished;
        }
    };

  public:
    MultiNumericsRangeView()
        requires(std::default_initializable<NumericsRangeType<Ranges>> and ...)
    = default;

    template<IsNumericsRange... R>
    constexpr MultiNumericsRangeView(R&&... ranges) noexcept
        requires(sizeof...(R) == sizeof...(Ranges))
        : m_size { (computeSizes(ranges) * ...) },
          m_starts { std::forward_like<R>(ranges.start)... },
          m_bounds { std::forward_like<R>(ranges.bound)... },
          m_strides { std::forward_like<R>(ranges.stride)... } {}

    template<typename Self>
    constexpr auto begin(this Self&& self) noexcept -> Iterator {
        return Iterator { std::forward_like<Self>(self.m_starts),
                          std::forward_like<Self>(self.m_bounds),
                          std::forward_like<Self>(self.m_strides) };
    }

    constexpr auto end() const noexcept -> Sentinel { return {}; }

    constexpr auto size() const noexcept -> std::size_t { return m_size; }

  private:
    std::size_t m_size;
    TupleType   m_starts;
    TupleType   m_bounds;
    TupleType   m_strides;
};

template<IsNumericsRange... Ranges>
MultiNumericsRangeView(Ranges...) -> MultiNumericsRangeView<Ranges...>;

namespace ranges {
    template<typename... Args>
    constexpr auto multiRange(Args&&... args) noexcept -> decltype(auto) {
        return MultiNumericsRangeView {
            NumericsRange<std::remove_cvref_t<Args>> { .start  = 0,
                                                      .bound  = std::forward<Args>(args),
                                                      .stride = 1 }
             ...
        };
    }
} // namespace ranges

#endif

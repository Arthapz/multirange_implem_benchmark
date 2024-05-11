#ifndef NUMERICS_RANGE_HPP
#define NUMERICS_RANGE_HPP

template<typename T>
struct NumericsRange {
    using Type = T;

    T start;
    T bound;
    T stride = T { 1 };
};

template<class T>
concept IsNumericsRange = requires(const T& t) {
    t.start;
    t.bound;
    t.stride;
};

template<IsNumericsRange Range>
using NumericsRangeType = Range::Type;

#endif

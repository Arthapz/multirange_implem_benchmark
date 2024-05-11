#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <print>

#include "coroutine.hpp"
#include "ranges.hpp"

using Int = std::int_fast64_t;

static constexpr auto WIDTH  = Int { 800 };
static constexpr auto HEIGHT = Int { 600 };
static constexpr auto DEPTH  = Int { 100 };

int main() {
    ankerl::nanobench::Bench().run("multiple for loops", []() noexcept {
        auto result = Int { 0 };

        for (auto x = Int { 0 }; x < WIDTH; ++x)
            for (auto y = Int { 0 }; y < HEIGHT; ++y)
                for (auto z = Int { 0 }; z < DEPTH; ++z) {
                    ankerl::nanobench::doNotOptimizeAway(result += (x + y + z));
                }
    });

    ankerl::nanobench::Bench().run("1D to 3D for loop", []() noexcept {
        auto result = Int { 0 };
        for (auto i = Int { 0 }; i < WIDTH * HEIGHT * DEPTH; ++i) {
            const auto x = i / (HEIGHT * DEPTH);
            const auto y = (i / DEPTH) % HEIGHT;
            const auto z = i % WIDTH;

            ankerl::nanobench::doNotOptimizeAway(result += (x + y + z));
        }
    });


    ankerl::nanobench::Bench().run("coroutine", []() noexcept {
        auto result = Int { 0 };
        for (auto&& [x, y, z] : coroutine::multiRange(WIDTH, HEIGHT, DEPTH)) {
            ankerl::nanobench::doNotOptimizeAway(result += (x + y + z));
        }
    });

    ankerl::nanobench::Bench().run("ranges", []() noexcept {
        auto result = Int { 0 };
        for (auto&& [x, y, z] : ranges::multiRange(WIDTH, HEIGHT, DEPTH)) {
            ankerl::nanobench::doNotOptimizeAway(result += (x + y + z));
        }
    });

    return 0;
}

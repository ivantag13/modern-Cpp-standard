#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <ranges>

int main()
{
    auto loop = std::views::iota(1, 51) | std::views::filter([](const auto x)
                                                             { return x % 2 == 1; });
    for (auto x : loop)
        std::cout << x << " ";
    std::cout << "\n";

    std::vector<int>
        v = {3, 5, 6, 10, 12};
    auto range = v | std::views::filter([](const auto &x)
                                        { auto &y = x;
                                            return y % 3 == 0; }) |
                 std::views::transform([](const auto &x)
                                       { auto &y = x;
                                         return y * y; });
    for (auto &&x : range)
        std::cout << x << " ";
    std::cout << "\n";

    auto test = std::views::iota(1, 101) | std::views::drop(50) | std::views::take(10) | std::views::transform([](const auto x)
                                                                                                               { return x * 2; });

    for (auto x : test)
        std::cout << x << " ";

        return 0;
}
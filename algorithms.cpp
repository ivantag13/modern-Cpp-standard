#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <concepts>
#include <type_traits>

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <arithmetic T>
void print_vector(const std::vector<T> &v)
{
    std::for_each(v.begin(), v.end(), [](T x)
                  { std::cout << x << " "; });
    std::cout << "\n";
};

template <arithmetic T>
void square_vector(std::vector<T> &v)
{
    std::transform(v.begin(), v.end(), v.begin(), [](T x)
                   { return x * x; });
};

template <arithmetic T>
void filter_great(const std::vector<T> &v, std::vector<T> &v_copy, T a)
{
    copy_if(v.begin(), v.end(), std::back_inserter(v_copy), [a](T x)
            { return x > a; });
};

template <arithmetic T>
T sum_vector(const std::vector<T> &v)
{
    return std::accumulate(v.begin(), v.end(), T{0});
};

int main()
{
    std::vector<int> values{1, 2, 3, 4, 5};
    print_vector(values);
    square_vector(values);
    print_vector(values);

    std::vector<bool> values_bool{false, true, true, false};
    print_vector(values_bool);

    std::vector<int> l(10), l_copy;
    std::iota(l.begin(), l.end(), 1);
    print_vector(l);
    square_vector(l);
    print_vector(l);
    std::cout << "Sum of squares: " << sum_vector(l) << "\n";

    filter_great(l, l_copy, 30);

    print_vector(l_copy);

    return 0;
}
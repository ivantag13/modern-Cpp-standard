#include <Kokkos_Core.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>

class TimerGuard
{
private:
    std::string description_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;

public:
    explicit TimerGuard(const std::string &description)
        : description_(description),
          start_(std::chrono::high_resolution_clock::now())
    {
        std::cout << "[TIMER] Starting: " << description_ << '\n';
    }

    ~TimerGuard()
    {
        auto end_ = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_ - start_;
        std::cout << "[TIMER]: " << description_ << " took " << elapsed.count() << " second \n";
    }

    TimerGuard(const TimerGuard &timer) = delete;
    TimerGuard &operator=(const TimerGuard &timer) = delete;
};

int main(int argc, char *argv[])
{
    Kokkos::ScopeGuard guard(argc, argv);
    std::cout << "Execution Space: " << typeid(Kokkos::DefaultExecutionSpace).name() << "\n";

    // Part 1: Practicing Matrices as Views
    Kokkos::View<double **, Kokkos::LayoutRight> m_d("matrix_device", 5, 5);
    Kokkos::parallel_for(m_d.extent(0) * m_d.extent(1), KOKKOS_LAMBDA(int id) {
        int i = id / m_d.extent(1);
        int j = id % m_d.extent(1);
        m_d(i,j) = i + j; });

    auto m_host = Kokkos::create_mirror_view(m_d);
    Kokkos::deep_copy(m_host, m_d);

    std::cout << "\nOriginal Matrix: \n";
    for (int i = 0; i < m_host.extent(0); i++)
    {
        for (int j = 0; j < m_host.extent(1); j++)
        {
            std::cout << m_host(i, j) << " ";
        }
        std::cout << "\n";
    }

    m_host(2, 2) = 999;

    Kokkos::deep_copy(m_d, m_host);

    Kokkos::deep_copy(m_host, m_d);

    std::cout << "\nModified Matrix: \n";
    for (int i = 0; i < m_host.extent(0); i++)
    {
        for (int j = 0; j < m_host.extent(1); j++)
        {
            std::cout << m_host(i, j) << " ";
        }
        std::cout << "\n";
    }

    // Part 2: Subview Pratice
    Kokkos::View<double **> zeros_d("zeros_d", 10, 10);
    Kokkos::deep_copy(zeros_d, 0);

    auto slice_of_1 = Kokkos::subview(zeros_d, std::make_pair(2, 8), std::make_pair(2, 8));
    Kokkos::deep_copy(slice_of_1, 1);

    auto slice_of_5 = Kokkos::subview(zeros_d, 5, Kokkos::ALL());
    Kokkos::deep_copy(slice_of_5, 5);

    auto zeros_host = Kokkos::create_mirror_view(zeros_d);
    Kokkos::deep_copy(zeros_host, zeros_d);

    std::cout << "\nMatrix of Zeros: \n";
    for (int i = 0; i < zeros_host.extent(0); i++)
    {
        for (int j = 0; j < zeros_host.extent(1); j++)
        {
            std::cout << zeros_host(i, j) << " ";
        }
        std::cout << "\n";
    }

    // Part 3: Deep Copy Semantics (Understanding shallow against deep copy)
    Kokkos::View<double *> a_d("a_d", 10);
    Kokkos::parallel_for(10, KOKKOS_LAMBDA(int i) { a_d(i) = i; });
    auto b_d = a_d; // Shallow copy (assignment)
    b_d(0) = 999;
    Kokkos::View<double *> c_d("c_d", 10);
    Kokkos::deep_copy(c_d, a_d);
    c_d(0) = 777;
    printf("\na_d(0) = %f, b_d(0) = %f, c_d(0) = %f\n", a_d(0), b_d(0), c_d(0));

    // Part 4: Using Different Memory Spaces
    Kokkos::View<double *, Kokkos::HostSpace> host_view("host_view", 10);
    for (int i = 0; i < 10; i++)
        host_view[i] = i;

    Kokkos::View<double *> device_view("device_view", 10);
    Kokkos::deep_copy(device_view, host_view);

    Kokkos::parallel_for(10, KOKKOS_LAMBDA(int i) { device_view[i] = 101 * i; });
    Kokkos::deep_copy(host_view, device_view);
    std::cout << "\nModified device_view and copied back to host_view: \n";
    for (int i = 0; i < 10; i++)
        std::cout << host_view[i] << " ";
    std::cout << "\n";

    // Part 5: Test Layout Experiments
    const int N = 1000;
    Kokkos::View<double **, Kokkos::LayoutLeft> m_left("m_left", N, N);
    Kokkos::View<double **, Kokkos::LayoutRight> m_right("m_right", N, N);
    std::cout << "\n=== Layout Experiment ===\n";

    // Test 1: Row-major access on LayoutRight (should be fast)
    {
        TimerGuard timer("LayoutRight with row-major access");
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                m_right(i, j) = i + j;
            }
        }
        Kokkos::fence(); // Ensure completion of Kokkos APIs (if any) before mesuring TimeGuard
    }

    // Test 2: Column-major access on LayoutRight (should be slow)
    {
        TimerGuard timer("LayoutRight with column-major access");
        for (int j = 0; j < N; ++j)
        {
            for (int i = 0; i < N; ++i)
            {
                m_right(i, j) = i + j;
            }
        }
        Kokkos::fence(); // Ensure completion of Kokkos APIs (if any) before mesuring TimeGuard
    }

    // Test 3: Row-major access on LayoutLeft (should be slow)
    {
        TimerGuard timer("LayoutLeft with row-major access");
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                m_left(i, j) = i + j;
            }
        }
        Kokkos::fence(); // Ensure completion of Kokkos APIs (if any) before mesuring TimeGuard
    }

    // Test 4: Column-major access on LayoutLeft (should be fast)
    {
        TimerGuard timer("LayoutLeft with column-major access");
        for (int j = 0; j < N; ++j)
        {
            for (int i = 0; i < N; ++i)
            {
                m_left(i, j) = i + j;
            }
        }
        Kokkos::fence(); // Ensure completion of Kokkos APIs (if any) before mesuring TimeGuard
    }

    return 0;
}
#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    Kokkos::ScopeGuard guard(argc, argv);

    std::cout << "Execution Space: " << typeid(Kokkos::DefaultExecutionSpace).name() << '\n';

    std::cout << "Kokkos on: :";

#ifdef KOKKOS_ENABLE_CUDA
    std::cout << "CUDA GPU \n";
#elif defined(KOKKOS_ENABLE_HIP)
    std::cout << "HIP GPU \n";
#elif defined(KOKKOS_ENABLE_OPENMP)
    std::cout << "OpenMP CPU\n";
#else
    std::cout << "Serial CPU\n";
#endif

    Kokkos::View<double *> data("myView", 20);
    // size() is for total amount of elements
    // extent(n) is used for elements in a given dimension, can take integer values, e.g., n=0, 1, 2, etc.
    std::cout << "View of size " << data.extent(0) << "\n";

    Kokkos::parallel_for(20, KOKKOS_LAMBDA(int i) { data(i) = i * i; });

    auto host = Kokkos::create_mirror_view(data);
    Kokkos::deep_copy(host, data);

    // Limitations within Kokkos
    // Views do not support range-based loops
    // But this is because of the powerful fact that Views can be multi-dimensional in Kokkos
    // Which is much more general
    // But at the same time reduce certain vector-like features from standard C++ 20, such as range-based loops
    std::cout << "Host-View after operation: ";
    for (int i = 0; i < host.extent(0); i++)
        std::cout << "host(" << i << ") = " << host(i) << "\n";
    std::cout << "\n";

    // Vector Scaling
    Kokkos::View<double *> a("a", 10);
    Kokkos::parallel_for(10, KOKKOS_LAMBDA(int i) { a(i) = i + 1; });
    Kokkos::View<double *> b("b", 10);
    Kokkos::parallel_for(10, KOKKOS_LAMBDA(int i) { b(i) = 2.8 * a(i); });

    auto a_host = Kokkos::create_mirror_view(a);
    auto b_host = Kokkos::create_mirror_view(b);
    Kokkos::deep_copy(a_host, a);
    Kokkos::deep_copy(b_host, b);

    for (int i = 0; i < 10; i++)
        std::cout << "a(" << i << ") = " << a_host(i) << ", b(" << i << ") = " << b_host(i) << "\n";

    // Matrices
    Kokkos::View<double **> m("m", 8, 5);
    Kokkos::parallel_for(m.extent(0) * m.extent(1), KOKKOS_LAMBDA(int id) {
        int i = id / m.extent(1);
        int j = id % m.extent(1);
        m(i, j) = i * m.extent(1) + j; // i.e., id
    });

    // Same here, serial code == for loop
    std::cout << "\n";
    auto m_host = Kokkos::create_mirror_view(m);
    Kokkos::deep_copy(m_host, m);
    for (int id = 0; id < m_host.extent(0) * m_host.extent(1); id++)
    {
        int i = id / m_host.extent(1);
        int j = id % m_host.extent(1);
        if (j == 0)
            std::cout << "\n";
        std::cout << m_host(i, j) << " ";
    }

    // Dot product
    double dot = 0;
    Kokkos::parallel_reduce(a.size(), KOKKOS_LAMBDA(int i, double &loop_dot) { loop_dot += a(i) * b(i); }, dot);
    std::cout << "\n\ndot = " << dot << "\n";

    return 0;
}
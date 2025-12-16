#include <Kokkos_Core.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
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

struct Stats
{
    double sum;
    double min;
    double max;
    int count;
};

struct MinMaxLoc
{
    double min_val;
    int min_loc;
    double max_val;
    int max_loc;
};

int main(int argc, char *argv[])
{
    // Initialize Execution Space
    Kokkos::ScopeGuard guard(argc, argv);
    std::cout << "Execution Space: " << typeid(Kokkos::DefaultExecutionSpace).name() << "\n";

    // parallel_for
    // - Kokkos::RangePolicy<`ExecutionSpace`, Kokkos::Rank<n>>({startRange.dim0, startRange.dim1,...},{endRange.dim0,endRange.dim1,...}).set_chunk_size(`integer you may want`)
    // Use labels in parallel nests !
    // parallel_reduce
    // parallel_scan
    // Kokkos::MDRangePolicy (Multiple Dimension)

    // Part 1: Vector Operations with parallel_reduce
    Kokkos::View<double *> vector("vector_reduce", 1000);

    Kokkos::parallel_for("filling vector_reduce", 1000, KOKKOS_LAMBDA(int i) { vector(i) = rand() / double(RAND_MAX); });

    Stats myStats{0.0, 1e100, -1e100, 0};
    Kokkos::parallel_reduce("reduce vector_reduce", 1000, KOKKOS_LAMBDA(int i, Stats &tempStats) {
        if (vector(i) < tempStats.min) {tempStats.min = vector(i);}
        if (vector(i) > tempStats.max) {tempStats.max = vector(i);}
        if (vector(i) > 0.5) {tempStats.count += 1;}
            tempStats.sum += vector(i); }, myStats);

    std::cout << "vector_reduce: \nsum = " << myStats.sum << "\nmin = " << myStats.min << "\nmax = " << myStats.max << "\ncount = " << myStats.count << "\naverage = " << myStats.sum / 1000 << "\n";

    // Part 2: Matrix Operations with MDRangePolicy
    Kokkos::View<double **> A("A", 100, 100);
    Kokkos::View<double **> B("B", 100, 100);
    Kokkos::View<double **> C("C", 100, 100);

    Kokkos::parallel_for("fill_A_B-find_C", Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {100, 100}), KOKKOS_LAMBDA(int i, int j) {
        A(i,j) = i + j;
        B(i,j) = i * j;
        C(i,j) = A(i,j) + B(i,j); });

    double sum = 0;
    Kokkos::parallel_reduce("Frobenius_C", 100 * 100, KOKKOS_LAMBDA(int id, double &tempSum) {
        int i = id / C.extent(1);
        int j = id % C.extent(1);
        tempSum += C(i,j) * C(i,j); }, sum);

    // Debugging
    // auto C_host = Kokkos::create_mirror_view(C);
    // Kokkos::deep_copy(C_host, C);

    // for (int i = 0; i < 30; i++)
    // {
    //     for (int j = 0; j < 30; j++)
    //     {
    //         std::cout << C_host(i, j) << " ";
    //     }
    //     std::cout << "\n";
    // }

    double norm = sqrt(sum);
    std::cout << "Frobenius norm of C " << norm << " " << sum << "\n";

    // Part 3: Prefix Sum Application
    Kokkos::View<int *> array("random_int", 10);
    Kokkos::View<int *> flag("flag", 10);
    Kokkos::View<int *> scan("scan", 10);
    Kokkos::deep_copy(flag, 0);
    Kokkos::parallel_for("filling_array", 10, KOKKOS_LAMBDA(int i) { 
        array(i) = (int)rand() / abs(int(RAND_MAX)/10); 
        if(array(i) > 5) flag(i) = 1; });

    Kokkos::parallel_scan("generate_index", 10, KOKKOS_LAMBDA(int i, int &update, bool final) {
        if(flag(i)) update++;    
        if(final) scan(i) = update; });

    auto array_host = Kokkos::create_mirror_view(array);
    Kokkos::deep_copy(array_host, array);
    auto flag_host = Kokkos::create_mirror_view(flag);
    Kokkos::deep_copy(flag_host, flag);
    auto scan_host = Kokkos::create_mirror_view(scan);
    Kokkos::deep_copy(scan_host, scan);

    std::cout << "Array: ";
    for (int i = 0; i < 10; i++)
        std::cout << array_host(i) << " ";
    std::cout << "\n";

    std::cout << "Flag: ";
    for (int i = 0; i < 10; i++)
        std::cout << flag_host(i) << " ";
    std::cout << "\n";

    std::cout << "Scan: ";
    for (int i = 0; i < 10; i++)
        std::cout << scan_host(i) << " ";
    std::cout << "\n";

    // Part 4: Dot Product (Production Version)
    const int n = 10000;
    Kokkos::View<double *> a("a", n);
    Kokkos::View<double *> b("b", n);
    Kokkos::parallel_for("fill_a_b", n, KOKKOS_LAMBDA(int i) {
        a(i) = i;
        b(i) = 2*i; });

    double dot = 0;
    {
        TimerGuard("Dot Product");
        Kokkos::parallel_reduce("dot_a_b", n, KOKKOS_LAMBDA(int i, double &tempDot) { tempDot += double(a(i)) * double(b(i)); }, dot);
        Kokkos::fence();
    }
    std::cout << "Dot Product a*b = " << dot << "\n";

    // Part 5: Finding Extrema with Location
    const int n_extrema = 100;
    Kokkos::View<double *> extrema("extrema", n_extrema);
    Kokkos::parallel_for("filling_extrema", n_extrema, KOKKOS_LAMBDA(int i) { extrema(i) = rand() / (double(RAND_MAX) / 100); });
    // MinMaxLoc myExtrema{1e100, 0, -1e100, 0};
    Kokkos::MinMaxLoc<double, int>::value_type result;

    Kokkos::parallel_reduce("finding_extrema", n_extrema, KOKKOS_LAMBDA(int i, Kokkos::MinMaxLoc<double, int>::value_type &update) { 
        if(extrema(i) < update.min_val){
            update.min_val = extrema(i);
            update.min_loc = i;
        }
        if(extrema(i) > update.max_val){
            update.max_val = extrema(i);
            update.max_loc = i;
        } }, Kokkos::MinMaxLoc<double, int>(result));

    std::cout << "Max value = " << result.max_val << " in position " << result.max_loc << "\n";
    std::cout << "Min value = " << result.min_val << " in position " << result.min_loc << "\n";

    auto extrema_host = Kokkos::create_mirror_view(extrema);
    Kokkos::deep_copy(extrema_host, extrema);
    std::cout << "Extrema_host[max_loc] = " << extrema_host(result.max_loc) << "\n";
    std::cout << "Extrema_host[min_loc] = " << extrema_host(result.min_loc) << "\n";

    return 0;
}
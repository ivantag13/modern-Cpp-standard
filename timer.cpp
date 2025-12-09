#include <chrono>
#include <iostream>
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

void expensive_operation()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void another_operation()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

int main()
{
    std::cout << "=== Test 1: Single timer ===\n";
    {
        TimerGuard timer("Test operation");
        expensive_operation();
    } // Timer destructor called here

    std::cout << "\n=== Test 2: Nested timers ===\n";
    {
        TimerGuard outer("Outer operation");
        {
            TimerGuard inner("Inner operation");
            another_operation();
        } // Inner timer ends here
        expensive_operation();
    } // Outer timer ends here

    std::cout << "\n=== Test 3: Early return ===\n";
    {
        TimerGuard timer("Operation with early return");
        expensive_operation();
        if (true)
        {
            std::cout << "Returning early...\n";
            return 0; // Timer still prints!
        }
        another_operation();
    }

    return 0;
}
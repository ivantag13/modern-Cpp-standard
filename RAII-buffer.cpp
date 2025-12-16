#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>

template <typename T>
class RAIIBuffer
{
private:
    T *data_;
    size_t size_;

public:
    // Constructor
    explicit RAIIBuffer(size_t n) : size_(n), data_(new T[n]) {}

    RAIIBuffer(std::initializer_list<T> list) : size_(list.size()), data_(new T[list.size()])
    {
        std::copy(list.begin(), list.end(), data_);
    }

    // Destructor
    ~RAIIBuffer()
    {
        delete[] this->data_;
        this->size_ = 0;
    }

    // Delete copy constructor and copy assignment
    // Useful for avoiding double delete, because it performs only shallow copies
    RAIIBuffer(const RAIIBuffer &buf) = delete;
    RAIIBuffer &operator=(const RAIIBuffer &buf) = delete;

    // Move constructor
    // This will allow us to, for instance, return buffers safely from functions
    // It is not necessary to do: data_(std::move(other.data_)) or size_(std::move(other.size_))
    // Because the first one is a pointer and the second is a primitive
    // These are variables with few semantics, therefore std::move will do nothing
    RAIIBuffer(RAIIBuffer &&other) noexcept : data_(other.data_), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // Move Assignement (Rule of Five is complete)
    RAIIBuffer &operator=(RAIIBuffer &&other) noexcept
    {
        if (this == &other)
            return *this;
        delete[] this->data_;
        this->data_ = other.data_;
        this->size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    // Access operator
    T &operator[](size_t index)
    {
        if (index >= this->size_)
            throw std::out_of_range("Index out of range\n");
        return this->data_[index];
    }

    const T &operator[](size_t index) const
    {
        if (index >= this->size_)
            throw std::out_of_range("Index out of range\n");
        return this->data_[index];
    }

    // Size getter
    size_t size() const { return this->size_; }
};

template <typename T, typename... Args>
RAIIBuffer<T> make_buffer(Args &&...args)
{
    // TODO: Create and return RAIIBuffer<T>, forwarding all arguments
    // Hint: RAIIBuffer<T>(std::forward<Args>(args)...)
    return RAIIBuffer<T>(std::forward<Args>(args)...);
}

int main()
{
    std::cout << "\n===== Testing Basic Usage RAIIBuffer class =====\n";
    // Test 1: Basic usage
    RAIIBuffer<int> buf(10);
    buf[0] = 42;
    buf[9] = 99;
    std::cout << "buf[0] = " << buf[0] << '\n';
    std::cout << "buf[9] = " << buf[9] << '\n';

    // Test 2: Automatic cleanup (no manual delete needed!)
    {
        RAIIBuffer<double> temp(5);
        temp[0] = 3.14;
        std::cout << "temp[0] = " << temp[0] << '\n';
        // temp is automatically destroyed here
    }
    std::cout << "temp was cleaned up automatically\n";

    // Test 3: Bounds checking
    try
    {
        buf[100] = 0; // Should throw!
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Caught exception: " << e.what() << '\n';
    }

    std::cout << "\n===== Testing Move Operation RAIIBuffer class =====\n";
    std::cout << "=== Test 1: Basic move ===\n";
    RAIIBuffer<int> buf1(10);
    buf1[0] = 42;
    buf1[9] = 99;

    std::cout << "Before move: buf1[0] = " << buf1[0] << ", size = " << buf1.size() << '\n';

    RAIIBuffer<int> buf2 = std::move(buf1); // Move constructor

    std::cout << "After move: buf2[0] = " << buf2[0] << ", size = " << buf2.size() << '\n';
    std::cout << "After move: buf1.size() = " << buf1.size() << '\n';

    std::cout << "\n=== Test 2: Return from function ===\n";
    auto create = []()
    {
        RAIIBuffer<int> temp(5);
        temp[0] = 100;
        return temp; // Move (or copy elision)
    };

    RAIIBuffer<int> buf3 = create();
    std::cout << "Returned buffer: buf3[0] = " << buf3[0] << ", size = " << buf3.size() << '\n';

    std::cout << "\n=== Test 3: Move into vector ===\n";
    std::vector<RAIIBuffer<int>> buffers;

    RAIIBuffer<int> buf4(3);
    buf4[0] = 11;
    buf4[1] = 22;
    buf4[2] = 33;

    buffers.push_back(std::move(buf4)); // Move into vector
    std::cout << "Vector element: " << buffers[0][0] << ", "
              << buffers[0][1] << ", " << buffers[0][2] << '\n';

    std::cout << "\n===== Testing Move Assignment RAIIBuffer class =====\n";
    {
        std::cout << "=== Test 1: Basic move assignment ===\n";
        RAIIBuffer<int> buf1(10);
        buf1[0] = 42;

        RAIIBuffer<int> buf2(20);
        buf2[0] = 99;

        std::cout << "Before: buf1[0] = " << buf1[0] << ", buf2[0] = " << buf2[0] << '\n';

        buf2 = std::move(buf1); // Move assignment

        std::cout << "After: buf2[0] = " << buf2[0] << ", buf2.size() = " << buf2.size() << '\n';
        std::cout << "After: buf1.size() = " << buf1.size() << '\n';

        std::cout << "\n=== Test 2: Self-assignment (should not crash) ===\n";
        RAIIBuffer<int> buf3(5);
        buf3[0] = 123;
        buf3 = std::move(buf3); // Self-assignment
        std::cout << "After self-assignment: buf3[0] = " << buf3[0] << '\n';

        std::cout << "\n=== Test 3: Chain assignment ===\n";
        RAIIBuffer<int> a(3), b(3), c(3);
        a[0] = 10;
        b[0] = 20;
        c[0] = 30;

        // This should work: a = std::move(b) = std::move(c);
        // But for clarity, let's do:
        b = std::move(c);
        a = std::move(b);
        std::cout << "a[0] = " << a[0] << '\n'; // Should be 30
    }

    std::cout << "\n===== Testing Perfect Forwarding in RAIIBuffer class =====\n";
    {
        std::cout << "=== Test 1: Direct construction ===\n";
        auto buf1 = make_buffer<int>(10);
        buf1[0] = 42;
        std::cout << "buf1[0] = " << buf1[0] << ", size = " << buf1.size() << '\n';

        std::cout << "\n=== Test 2: Using size from variable ===\n";
        size_t n = 20;
        auto buf2 = make_buffer<double>(n);
        buf2[0] = 3.14;
        std::cout << "buf2[0] = " << buf2[0] << ", size = " << buf2.size() << '\n';

        std::cout << "\n=== Test 3: Move from rvalue ===\n";
        auto buf3 = make_buffer<int>(size_t(5)); // size_t(5) is rvalue
        buf3[0] = 100;
        std::cout << "buf3[0] = " << buf3[0] << ", size = " << buf3.size() << '\n';

        std::cout << "\n=== Test 4: make_buffer with initializer lists ===\n";
        // auto buf4 = make_buffer<int>({1, 2, 3, 4, 5});
        // Why the above does not work. From Claude:
        // Template deduction needs a type from the argument
        // {1, 2, 3} is not an expression and has no type
        // Therefore, template deduction cannot work
        RAIIBuffer<int> buf4 = {1, 2, 3, 4, 5};
        std::cout << "buf4[2] = " << buf4[2] << ", size = " << buf4.size() << '\n';
    }

    return 0;
}
#include <iostream>
#include <stdexcept>

template <typename T>
class RAIIBuffer
{
private:
    T *data_;
    size_t size_;

public:
    // Constructor
    explicit RAIIBuffer(size_t n) : size_(n), data_(new T[n]) {}

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

int main()
{
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

    return 0;
}
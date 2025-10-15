#include <iostream>

class Point {
private:
    float x, y; // Encapsulated data

public:
    // Constructor
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}

    // Method
    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    // Method to print
    void print() const {
        std::cout << "(" << x << ", " << y << ")\n";
    }
};

int main() {
    Point p(2.0f, 3.0f); // calls constructor
    p.move(1.0f, -2.0f);
    p.print();           // prints (3, 1)
}

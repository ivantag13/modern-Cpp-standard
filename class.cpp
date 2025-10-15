#include <iostream>
#include <cmath>

class Point
{
private:
    float x, y; // Encapsulated data

public:
    // Constructor
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}

    ~Point() { std::cout << "Point destroyed.\n"; }

    // Method
    void move(float dx, float dy)
    {
        x += dx;
        y += dy;
    }

    float distance_to(const Point &other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Method to print
    void print() const
    {
        std::cout << "(" << x << ", " << y << ")\n";
    }
};

class Rectangle
{
    float width, height;

public:
    Rectangle(float w, float h) : width(w), height(h) {} // initialization list for efficiency

    float area() const { return width * height; }
};

int main()
{
    Point p(2.0f, 3.0f); // calls constructor
    p.move(1.0f, -2.0f);
    p.print(); // prints (3, 1)

    Point *p_p = new Point(1, 2); // allocated on heap
    p_p->move(4.9, 0.3);
    p_p->print();

    float distance_p_p_to_p = p_p->distance_to(p);
    float distance_p_to_p_p = p.distance_to(*p_p);

    std::cout << "Distance from p_p to p: " << distance_p_p_to_p << "\n";
    std::cout << "Distance from p to p_p: " << distance_p_to_p_p << "\n";
    delete p_p; // manually destroyed

    Rectangle r(2.3f, 4.1f);
    float area = r.area();
    std::cout << "Area: " << area << " \n";

    return 0;
}

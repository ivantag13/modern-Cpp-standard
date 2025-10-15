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

    // Accessors
    float getX() const { return this->x; }
    float getY() const { return this->y; }

    void setX(float val) { this->x = val; }
    void setY(float val) { this->y = val; }

    // Method
    void translate(float dx, float dy)
    {
        x += dx;
        y += dy;
    }

    float norm(Point p) const
    {
        return std::sqrt(p.getX() * p.getX() + p.getY() * p.getY());
    }

    Point operator+(const Point &other) const
    {
        return Point(this->x + other.getX(), this->y + other.getY());
    }

    Point operator-(const Point &other) const
    {
        return Point(this->x - other.getX(), this->y - other.getY());
    }

    bool operator==(const Point &other) const
    {
        return x == other.getX() && y == other.getY();
    }

    float distance_to(const Point &other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        Point p(dx, dy);
        return norm(p);
    }

    // Method to print
    void print() const
    {
        std::cout << "(" << x << ", " << y << ")\n";
    }
};

class LineSegment
{
private:
    Point p1, p2;

public:
    LineSegment(const Point &p1_val, const Point &p2_val) : p1(p1_val), p2(p2_val) {}

    ~LineSegment() { std::cout << "Line segment destroyed\n"; }

    float length() const
    {
        return p1.distance_to(p2);
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
    p.translate(1.0f, -2.0f);
    p.print(); // prints (3, 1)

    Point *p_p = new Point(1, 2); // allocated on heap
    p_p->translate(4.9, 0.3);
    p_p->print();

    Point c = p + *p_p;
    std::cout << "Print point c: ";
    c.print();

    Point d = p - *p_p;
    std::cout << "Print point d: ";
    d.print();

    std::cout << "Is c equal to d: " << (d == c) << "\n";

    float distance_p_p_to_p = p_p->distance_to(p);
    float distance_p_to_p_p = p.distance_to(*p_p);

    std::cout << "Distance from p_p to p: " << distance_p_p_to_p << "\n";
    std::cout << "Distance from p to p_p: " << distance_p_to_p_p << "\n";
    LineSegment l(p, *p_p);
    std::cout << "Lenght of l: " << l.length() << "\n";

    delete p_p; // manually destroyed

    Rectangle r(2.3f, 4.1f);
    float area = r.area();
    std::cout << "Area: " << area << " \n";

    return 0; // RAII (Resource Acquisition Is Initialization) takes care of freeing any used memory
}

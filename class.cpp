#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <memory>
#include <string>

template <typename T>
T max_value(T a, T b)
{
    return (a > b) ? a : b;
}

class Point
{
private:
    float x, y; // Encapsulated data

public:
    // Empty constructor
    Point() : x(), y() {}

    // Constructor
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}

    // Copy constructor
    Point(const Point &other) : x(other.getX()), y(other.getY())
    {
        // std::cout << "Copied Point\n";
    }

    ~Point()
    {
        // std::cout << "Point destroyed.\n";
    }

    // Accessors
    float getX() const { return this->x; }
    float getY() const { return this->y; }

    void setX(float val) { this->x = val; }
    void setY(float val) { this->y = val; }

    // Method
    void translate(float dx, float dy)
    {
        this->x += dx;
        this->y += dy;
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

    // Copy assignement operator
    Point &operator=(const Point &other)
    {
        if (this != &other)
        { // Avoids self assignment by checking the same memory space addresses
            this->x = other.getX();
            this->y = other.getY();
        }
        std::cout << "Assigned Point \n";
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const Point &p);

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

std::ostream &operator<<(std::ostream &os, const Point &p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

class LineSegment
{
private:
    Point p1, p2;

public:
    LineSegment() : p1(), p2() {}

    LineSegment(const Point &p1_val, const Point &p2_val) : p1(p1_val), p2(p2_val) {}

    LineSegment(const LineSegment &other) : p1(other.p1), p2(other.p2) {}

    ~LineSegment()
    {
        // std::cout << "Line segment destroyed\n";
    }

    void translate(float dx, float dy)
    {
        this->p1.translate(dx, dy);
        this->p2.translate(dx, dy);
    }

    LineSegment &operator=(const LineSegment &other)
    {
        if (this != &other)
        {
            this->p1 = other.p1;
            this->p2 = other.p2;
        }
        return *this;
    }

    bool operator==(const LineSegment &other) const
    {
        return this->p1 == other.p1 && this->p2 == other.p2;
    }

    friend std::ostream &operator<<(std::ostream &os, const LineSegment &line);

    float length() const
    {
        return p1.distance_to(p2);
    }
};

std::ostream &operator<<(std::ostream &os, const LineSegment &line)
{
    os << "[ " << line.p1 << " -> " << line.p2 << " ]";
    return os;
}

class Rectangle
{
    float width, height;

public:
    Rectangle(float w, float h) : width(w), height(h) {} // initialization list for efficiency

    float area() const { return width * height; }
};

class ShapeCollection
{
private:
    std::vector<LineSegment> shape;

public:
    // Empty constructor
    ShapeCollection() = default;
    // The same as:
    // ShapeCollection() : shape() {}

    // Constructor
    void add(const LineSegment &line)
    {
        this->shape.emplace_back(line);
    }

    void translate_all(const Point &delta)
    {
        for (auto &line : this->shape)
            line.translate(delta.getX(), delta.getY());
    }

    void print_all() const
    {
        for (const auto &line : this->shape)
            std::cout << line << "\n";
    }

    ~ShapeCollection()
    {
        std::cout << "Shape destroyed.\n";
    }
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
    std::cout << "Print point c: " << c << "\n";

    Point d = p - *p_p;
    std::cout << "Print point d: ";
    d.print();

    Point e;
    e = d;
    std::cout << "Point d assigned to e: " << e << "\n";

    Point f(e);
    std::cout << "Point f constructed using point e: " << f << "\n";

    std::cout << "Is c equal to d: " << (d == c) << "\n";

    float distance_p_p_to_p = p_p->distance_to(p);
    float distance_p_to_p_p = p.distance_to(*p_p);

    std::cout << "Distance from p_p to p: " << distance_p_p_to_p << "\n";
    std::cout << "Distance from p to p_p: " << distance_p_to_p_p << "\n";
    LineSegment l(p, *p_p);
    std::cout << "Lenght of l: " << l.length() << "\n";
    LineSegment t(p, d);
    LineSegment k;
    k = t;

    std::cout << "Is t " << t << " == l " << l << " : " << (t == l) << "\n";
    std::cout << "Is t" << t << " == k " << k << " : " << (t == k) << "\n";

    k.translate(c.getX(), c.getY());

    std::cout << "Is t " << t << " == k (translated) " << k << " : " << (t == k) << "\n";

    delete p_p; // manually destroyed

    Rectangle r(2.3f, 4.1f);
    float area = r.area();
    std::cout << "Area: " << area << " \n";

    ShapeCollection shape;
    shape.add(l);
    shape.add(t);
    shape.add(k);

    std::cout << "Original Collection of Shapes: \n";
    shape.print_all();
    shape.translate_all(d);
    std::cout << "Translated Collection of Shapes by d: " << d << "\n";
    shape.print_all();

    return 0; // RAII (Resource Acquisition Is Initialization) takes care of freeing any used memory
}

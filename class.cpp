#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <memory>
#include <string>

template <typename T>
T max_value(const T a, const T b)
{
    return (a > b) ? a : b;
}

template <typename T>
T average(const std::vector<T> &v)
{
    T sum = {};
    for (auto &x : v)
        sum += x;
    return sum / v.size();
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

template <typename T>
class ShapeCollection
{
private:
    std::vector<T> shapes;

public:
    // Empty constructor
    ShapeCollection() = default;
    // The same as:
    // ShapeCollection() : shape() {}

    // Constructor
    void add(const T &elem)
    {
        this->shapes.emplace_back(elem);
    }

    void translate_all(const Point &delta)
    {
        for (auto &elem : this->shapes)
            elem.translate(delta.getX(), delta.getY());
    }

    void print_all() const
    {
        for (const auto &elem : this->shapes)
            std::cout << elem << "\n";
    }

    ~ShapeCollection()
    {
        std::cout << "Shape destroyed.\n";
    }
};

template <typename T>
class ShapeCollectionShared
{
private:
    std::vector<std::shared_ptr<T>> shapes;

public:
    ShapeCollectionShared() = default;

    void add(const std::shared_ptr<T> &shape)
    {
        shapes.push_back(shape);
    }

    void translate_all(const Point &delta)
    {
        for (auto &s : shapes)
            s->translate(delta.getX(), delta.getY()); // use -> for shared_ptr
    }

    void print_all() const
    {
        for (const auto &s : shapes)
            std::cout << *s << "\n"; // dereference for printing
    }

    ~ShapeCollectionShared()
    {
        std::cout << "Shared ShapeCollection destroyed\n";
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

    ShapeCollection<Point> pointCloud;
    ShapeCollection<LineSegment> lineCloud;

    pointCloud.add(p);
    pointCloud.add(*p_p);

    lineCloud.add(l);
    lineCloud.add(t);
    lineCloud.add(k);

    std::cout << "Original Collection of Points: \n";
    pointCloud.print_all();
    pointCloud.translate_all(d);
    std::cout << "Translated Collection of Points by d: " << d << "\n";
    pointCloud.print_all();

    std::cout << "Original Collection of Lines: \n";
    lineCloud.print_all();
    lineCloud.translate_all(d);
    std::cout << "Translated Collection of Lines by d: " << d << "\n";
    lineCloud.print_all();

    std::vector<int> v = {3, 4, 1, 2, 9};
    std::cout << "Point p: " << p << " with Max coordinates : " << max_value(p.getX(), p.getY()) << "\n";
    std::cout << "Vector v: ";
    for (auto &x : v)
        std::cout << x << " ";
    std::cout << ", with average = " << average(v) << "\n";

    auto l1 = std::make_shared<LineSegment>(Point(0, 0), Point(1, 1));
    auto l2 = std::make_shared<LineSegment>(Point(2, 2), Point(3, 3));

    ShapeCollectionShared<LineSegment> sharedLines;
    sharedLines.add(l1);
    sharedLines.add(l2);
    std::cout << "Original Shared Collection of Lines: \n";
    sharedLines.print_all();

    sharedLines.translate_all(Point(1, 0));
    std::cout << "Translated Shared Collection of Lines by : " << Point(1, 0) << "\n";
    sharedLines.print_all();

    return 0; // RAII (Resource Acquisition Is Initialization) takes care of freeing any used memory
}

#include <iostream>
#include <optional>
#include <string>
#include <vector>

class Student
{
private:
    std::string name_;
    double average_;

public:
    Student(std::string name, double average) : name_(name), average_(average) {};

    Student() : name_(), average_() {}

    ~Student() {};

    // Accessors
    auto getName() const { return this->name_; }
    auto getAvg() const { return this->average_; }

    // Delete Initialization by Copy
    // Student(const Student &student) = delete;
    // Delete Assignement Operator
    // Student &operator=(const Student &student) = delete;
};

std::optional<double> find_student_average(const std::vector<Student> &students, const std::string &name)
{
    for (const auto &stu : students)
    {
        if (stu.getName() == name)
            return stu.getAvg();
    }
    return std::nullopt;
}

std::optional<Student> find_best_student(const std::vector<Student> &students)
{
    if (students.empty())
        return std::nullopt;
    Student best = students[0];
    for (const auto &stu : students)
    {
        if (best.getAvg() < stu.getAvg())
            best = stu;
    }
    return best;
}

int main()
{
    std::vector<Student> students = {
        {"Alice", 8.5},
        {"Bob", 7.2},
        {"Charlie", 9.1}};

    // Search for Alice
    auto result = find_student_average(students, "Alice");
    if (result)
        std::cout << "Alice's average: " << *result << '\n';
    else
        std::cout << "Alice not found\n";

    // Search for David
    result = find_student_average(students, "David");
    if (result)
        std::cout << "David's average: " << *result << '\n';
    else
        std::cout << "David not found\n";

    // Test find_best_student
    std::cout << "\n=== Test find_best_student ===\n";

    auto best = find_best_student(students);
    if (best)
    {
        std::cout << "Best student: " << best->getName()
                  << " (average: " << best->getAvg() << ")\n";
    }
    else
    {
        std::cout << "No students in list\n";
    }

    // Test with empty vector
    std::cout << "\n=== Test with empty vector ===\n";
    std::vector<Student> empty_list;
    auto best2 = find_best_student(empty_list);
    if (best2)
    {
        std::cout << "Best student: " << best2->getName() << '\n';
    }
    else
    {
        std::cout << "No students in list\n";
    }

    return 0;
}
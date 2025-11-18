#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <vector>
#include <memory>
#include <string>

// class GradeTracker
// {
// private:
//     std::map<std::string, std::vector<int>> tracker;

// public:
//     // Empty Constructor
//     GradeTracker() : tracker() {}

//     void add_grade(std::map<std::string, std::vector<int>> tracker, std::string name, int grade)
//     {
//         tracker[name].push_back(grade);
//     }

//     // Deconstructor
//     ~GradeTracker() {}
// };

void add_grade(std::map<std::string, std::vector<int>> &tracker, std::string name, int grade)
{
    tracker[name].push_back(grade);
}

void avg_per_student(std::map<std::string, std::vector<int>> &tracker)
{
    for (const auto &[name, v] : tracker)
    {
        std::cout << "Average grade for " << name << ": ";
        std::cout << std::accumulate(v.begin(), v.end(), 0) / v.size() << "\n";
    }
}

void descending_avg(std::map<std::string, std::vector<int>> &tracker)
{
    auto cmp = [](const auto &a, const auto &b)
    {
        if (a.second != b.second)
            return a.second > b.second;
        else
            return a.first < b.first;
    };
    std::set<std::pair<std::string, double>, decltype(cmp)> desc_set(cmp);
    for (const auto &[name, v] : tracker)
    {
        double sum = static_cast<double>(std::accumulate(v.begin(), v.end(), double{0}));
        double avg = sum / static_cast<double>(v.size());
        desc_set.insert({name, avg});
    }
    for (const auto &[name, avg] : desc_set)
        std::cout << "Student " << name << ": " << avg << " \n";
}

int main()
{
    std::map<std::string, std::vector<int>> tracker;

    add_grade(tracker, "Alice", 8);
    add_grade(tracker, "Alice", 4);

    add_grade(tracker, "Julio", 10);
    add_grade(tracker, "Ana", 7);
    add_grade(tracker, "Ana", 8);
    add_grade(tracker, "Ana", 5);

    for (const auto &[name, v] : tracker)
    {
        std::cout << "Grade for " << name << ": ";
        for (const auto &grade : v)
            std::cout << grade << " ";
    }
    std::cout << "\n";

    avg_per_student(tracker);

    descending_avg(tracker);

    return 0;
}
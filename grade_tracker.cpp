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
#include <ranges>

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

void add_grade(std::map<std::string, std::vector<int>> &tracker, const std::string &name, const int grade)
{
    tracker[name].push_back(grade);
}

void avg_per_student(const std::map<std::string, std::vector<int>> &tracker)
{
    for (const auto &[name, v] : tracker)
    {
        double sum = static_cast<double>(std::accumulate(v.begin(), v.end(), double{0}));
        double avg = sum / static_cast<double>(v.size());
        std::cout << name << " -> " << avg << "\n";
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
        std::cout << name << " -> " << avg << " \n";
}

int main()
{
    std::map<std::string, std::vector<int>> tracker;

    add_grade(tracker, "Alice", 8);
    add_grade(tracker, "Alice", 4);

    add_grade(tracker, "Julio", 10);
    add_grade(tracker, "Ana", 7);
    add_grade(tracker, "Ana", 8);
    add_grade(tracker, "Ana", 10);

    std::cout << "Grades: \n";
    for (const auto &[name, v] : tracker)
    {
        std::cout << name << " -> ";
        for (const auto &grade : v)
            std::cout << grade << " ";
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout << "Students averages (alphabetical): \n";
    avg_per_student(tracker);

    std::cout << "Students averages (descending): \n";
    descending_avg(tracker);

    // Something similar as before but using views and ranges
    std::cout << "Students averages greater than 7 (descending order): \n";

    auto averages = tracker | std::views::transform([](const auto &pair)
                                                    { auto& [name, grades] = pair;
                                                        int sum = std::accumulate(grades.begin(), grades.end(), 0);
                                                        int avg = sum / grades.size();
                                                        return std::pair{name, avg}; }) |
                    std::views::filter([](const auto &pair)
                                       { return pair.second > 7; });

    std::vector<std::pair<std::string, int>> result;
    for (auto &&p : averages)
        result.push_back(std::move(p));

    std::ranges::sort(result, [](const auto &a, const auto &b)
                      { if(a.second != b.second)
                            return a.second > b.second;
                        else
                            return a.first < b.first; });

    for (auto &p : result)
        std::cout << p.first << " -> " << p.second << "\n";

    return 0;
}
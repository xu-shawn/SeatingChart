#include <fstream>
#include <iostream>

#include "parse.hpp"
#include "seatingchart.hpp"
#include "simulation.hpp"

using namespace SeatingChart;

int main() {
    auto [a, b, c] = parse<6, 8>(std::ifstream("6_by_8.txt"));
    for (const auto& name : a)
    {
        std::cout << name << std::endl;
    }
}

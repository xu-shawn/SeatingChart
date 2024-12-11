#include <fstream>
#include <iostream>

#include "parse.hpp"
#include "seatingchart.hpp"
#include "simulation.hpp"

using namespace SeatingChart;

int main() {
    constexpr std::size_t Row    = 6;
    constexpr std::size_t Column = 8;
    auto [names_lookup, seating_chart, class_info] =
      parse<Row, Column>(std::ifstream("6_by_8.txt"));
    Simulation<Row, Column> sim{seating_chart, class_info, 100};
}

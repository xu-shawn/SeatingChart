#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>

#include "parse.hpp"
#include "seatingchart.hpp"
#include "simulation.hpp"

using namespace SeatingChartGenetic;

int main() {
    constexpr std::size_t Row    = 6;
    constexpr std::size_t Column = 8;
    auto [names_lookup, seating_chart, class_info] =
      parse<Row, Column>(std::ifstream("6_by_8.txt"));
    std::cout << score_chart(seating_chart, class_info) << std::endl;

    auto scoring_function = [&class_info = class_info](const SeatingChart<Row, Column> chart) {
        return score_chart(chart, class_info);
    };

    std::default_random_engine rng{42};

    for (std::size_t i = 0; i < std::numeric_limits<std::size_t>::max(); i++)
    {
        seating_chart.random_shuffle(rng);

        while (seating_chart.hill_climb_combined(scoring_function))
        {}

        std::cout << score_chart(seating_chart, class_info) << std::endl;
    }
}

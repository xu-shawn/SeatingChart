#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "export.hpp"
#include "parse.hpp"
#include "seatingchart.hpp"
#include "simulation.hpp"

using namespace SeatingChartGenetic;

int main() {
    constexpr std::size_t Row      = 6;
    constexpr std::size_t Column   = 8;
    constexpr std::size_t Patience = 1500;

    auto [names_lookup, seating_chart, class_info] =
      parse<Row, Column>(std::ifstream("6_by_8.txt"));

    std::cout << "Patience: " << Patience << std::endl;


    auto scoring_function = [&class_info = class_info](const SeatingChart<Row, Column> chart) {
        return score_chart(chart, class_info);
    };

    std::default_random_engine rng{std::random_device{}()};
    double                     best_value                  = -1000;
    std::size_t                iterations_since_last_raise = 0;

    for (std::size_t i = 0; i < std::numeric_limits<std::size_t>::max(); i++)
    {
        seating_chart.partial_random_shuffle<decltype(rng), 12>(rng);
        iterations_since_last_raise++;

        while (seating_chart.hill_climb_combined(scoring_function))
        {}

        const double curr_value = score_chart(seating_chart, class_info);

        if (curr_value > best_value)
        {
            export_chart(
              seating_chart, names_lookup,
              std::ofstream(std::to_string(curr_value) + "_" + std::to_string(i) + ".txt"));

            std::cout << "New High: " << curr_value << std::endl;

            best_value                  = curr_value;
            iterations_since_last_raise = 0;
        }

        if (iterations_since_last_raise > Patience)
        {
            std::cout << "Stuck? Resetting..." << std::endl;
            seating_chart.random_shuffle(rng);
            best_value                  = -1000;
            iterations_since_last_raise = 0;
        }
    }
}

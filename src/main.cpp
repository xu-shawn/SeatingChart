#include <fstream>
#include <iostream>

#include "parse.hpp"
#include "seatingchart.hpp"
#include "simulation.hpp"

using namespace SeatingChartTSP;

int main() {
    constexpr std::size_t Row    = 6;
    constexpr std::size_t Column = 8;
    auto [names_lookup, seating_chart, class_info] =
      parse<Row, Column>(std::ifstream("6_by_8.txt"));
    std::cout << score_chart(seating_chart, class_info) << std::endl;
    std::default_random_engine rng(42);

    double best = -100000;

    for (int k = 0; k < 200; k++)
    {
        for (int i = 0; i < 300; i++)
        {
            seating_chart.hill_climb([&class_info](const SeatingChart<Row, Column>& chart) {
                return score_chart(chart, class_info);
            });
        }

        for (int i = 0; i < 300; i++)
        {
            seating_chart.hill_climb_pair([&class_info](const SeatingChart<Row, Column>& chart) {
                return score_chart(chart, class_info);
            });
        }

        double temp = score_chart(seating_chart, class_info);

        std::cout << temp << std::endl;

        if (temp > best)
            best = temp;
        else
        {
            best = -10000;
            seating_chart.random_shuffle(rng);
        }
    }

    Simulation<Row, Column> sim{seating_chart, class_info, 128};
    for (int i = 0; i < 100000; i++)
    {
        sim.step();
        if (i % 1000 == 0)
            std::cout << sim.top().score << std::endl;
    }

    std::cout << sim.top().score;
}

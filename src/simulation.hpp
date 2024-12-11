#ifndef SIMULATION_HPP_INCLUDED
#define SIMULATION_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <random>
#include <vector>

#include "classinfo.hpp"
#include "seatingchart.hpp"

namespace SeatingChart {

namespace {

constexpr int distance_squared(const Location location1, const Location location2) {
    return (location1.row - location2.row) * (location1.row - location2.row)
         + (location1.column - location2.column) * (location1.column - location2.column);
}

}

struct SimulationInfo {
    double best_score;
};

template<std::size_t Row, std::size_t Column>
struct ScoredChart {
    SeatingChart<Row, Column> chart;
    double                    score;
    ScoredChart(const SeatingChart<Row, Column>, const double score);
};

template<std::size_t Row, std::size_t Column>
auto operator<=>(const ScoredChart<Row, Column>&, const ScoredChart<Row, Column>&);

template<std::size_t Row, std::size_t Column>
class Simulation {
    std::vector<ScoredChart<Row, Column>> population;
    ClassInfo<Row * Column>               class_info;
    std::default_random_engine            rng;

   public:
    Simulation(const SeatingChart<Row, Column>&, const ClassInfo<Row * Column>&, std::size_t);
    SimulationInfo                  step() noexcept;
    const ScoredChart<Row, Column>& top() const noexcept;
};

template<std::size_t Row, std::size_t Column>
[[nodiscard]] constexpr double score_chart(const SeatingChart<Row, Column>&,
                                           const ClassInfo<Row * Column>&) noexcept;

}

namespace SeatingChart {

template<std::size_t Row, std::size_t Column>
ScoredChart<Row, Column>::ScoredChart(SeatingChart<Row, Column> c, double s) :
    chart{c},
    score{s} {}

template<std::size_t Row, std::size_t Column>
auto operator<=>(const ScoredChart<Row, Column>& chart, const ScoredChart<Row, Column>& other) {
    return chart.score <=> other.score;
}

template<std::size_t Row, std::size_t Column>
Simulation<Row, Column>::Simulation(const SeatingChart<Row, Column>& seed,
                                    const ClassInfo<Row * Column>&   cinfo,
                                    std::size_t                      cnt) :
    class_info{cinfo},
    rng{42} {
    population.reserve(cnt);

    for (int i = 0; i < cnt; i++)
    {
        population.emplace_back(seed, 0);
        population.back().chart.random_shuffle(rng);
    }
}

template<std::size_t Row, std::size_t Column>
SimulationInfo Simulation<Row, Column>::step() noexcept {
    using std::begin, std::end, std::cbegin, std::size;
    using distribution_type = std::uniform_int_distribution<typename decltype(rng)::result_type>;

    distribution_type dist{0, 100};

    SimulationInfo ret;

    for (auto& chart : population)
        chart.score = score_chart(chart.chart, class_info);

    std::sort(begin(population), end(population), std::greater{});

    ret.best_score = population[0].score;

    auto it = cbegin(population);
    for (std::size_t i = size(population) / 2; i < size(population); i++)
    {
        population[i] = *it;
        ++it;
    }

    for (std::size_t i = 1; i < size(population); i++)
        if (population[i].score > 150)
        {
            if (dist(rng) > 70)
                population[i].chart.mutate(rng);
            else
                population[i].chart.mutate2(rng);
        }
        else
            population[i].chart.mutate(rng);

    return ret;
}

template<std::size_t Row, std::size_t Column>
const ScoredChart<Row, Column>& Simulation<Row, Column>::top() const noexcept {
    return population[0];
}

template<std::size_t Row, std::size_t Column>
constexpr double score_chart(const SeatingChart<Row, Column>& chart,
                             const ClassInfo<Row * Column>&   class_info) noexcept {
    double total_score = 0;

    for (const auto& row : chart.seats())
    {
        for (const auto student : row)
        {
            if (class_info.friends_towards(student, chart.get_tablemate(student)))
                total_score += 5;

            if (class_info.enemies_towards(student, chart.get_tablemate(student)))
                total_score -= 5;

            for (const auto stu_friend : class_info.friends_of(student))
            {
                total_score +=
                  4.0 / distance_squared(chart.locations()[student], chart.locations()[stu_friend]);
            }

            for (const auto stu_enemy : class_info.enemies_of(student))
            {
                total_score -=
                  3.0 / distance_squared(chart.locations()[student], chart.locations()[stu_enemy]);
            }
        }
    }

    return total_score;
}

}

#endif

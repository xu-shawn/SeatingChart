#ifndef SEATINGCHART_HPP_INCLUDED
#define SEATINGCHART_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <random>
#include <type_traits>
#include <iostream>

namespace SeatingChartGenetic {

struct Location {
    std::size_t row;
    std::size_t column;
};

template<std::size_t Row, std::size_t Column>
class SeatingChart {
    std::array<std::array<std::size_t, Column>, Row> seats_;
    std::array<Location, Row * Column>               locations_;

    constexpr void swap_students(std::size_t, std::size_t) noexcept;
    constexpr void swap_pairs(std::size_t, std::size_t) noexcept;

   public:
    template<typename T,
             typename = typename std::enable_if_t<
               std::is_same_v<std::remove_reference_t<T>,
                              std::array<std::array<std::size_t, Column>, Row>>,
               bool>>
    constexpr SeatingChart(T&&);
    constexpr SeatingChart(const SeatingChart&) = default;

    [[nodiscard]] constexpr const auto&        seats() const noexcept { return seats_; }
    [[nodiscard]] constexpr const auto&        locations() const noexcept { return locations_; }
    [[nodiscard]] static constexpr std::size_t tablemate_column(std::size_t column) noexcept {
        return column - 2 * (column % 2) + 1;
    }
    [[nodiscard]] constexpr std::size_t get_tablemate(std::size_t) const noexcept;

    template<typename PRNG>
    void random_shuffle(PRNG&);

    template<typename Scorer>
    bool hill_climb_students(Scorer&);

    template<typename Scorer>
    bool hill_climb_pairs(Scorer&);

    template<typename Scorer>
    bool hill_climb_combined(Scorer&);
};

}

namespace SeatingChartGenetic {

template<std::size_t Row, std::size_t Column>
template<typename T, typename>
constexpr SeatingChart<Row, Column>::SeatingChart(T&& c) :
    seats_(std::forward<T>(c)) {
    for (std::size_t i = 0; i < Row; i++)
        for (std::size_t j = 0; j < Column; j++)
            locations_[seats_[i][j]] = {i, j};
}

template<std::size_t Row, std::size_t Column>
constexpr void SeatingChart<Row, Column>::swap_students(std::size_t first,
                                                        std::size_t second) noexcept {
    using std::swap;
    assert(first >= 0 && first < Row * Column && second >= 0 && first < Row * Column);
    const auto [first_row, first_column]   = locations_[first];
    const auto [second_row, second_column] = locations_[second];

    swap(seats_[first_row][first_column], seats_[second_row][second_column]);
    swap(locations_[first], locations_[second]);
}

template<std::size_t Row, std::size_t Column>
constexpr void SeatingChart<Row, Column>::swap_pairs(std::size_t first,
                                                     std::size_t second) noexcept {
    this->swap_students(first, second);
    this->swap_students(get_tablemate(first), get_tablemate(second));
}

template<std::size_t Row, std::size_t Column>
constexpr std::size_t SeatingChart<Row, Column>::get_tablemate(std::size_t student) const noexcept {
    const auto [row, column] = locations_[student];
    return seats_[row][tablemate_column(column)];
}

template<std::size_t Row, std::size_t Column>
template<typename PRNG>
void SeatingChart<Row, Column>::random_shuffle(PRNG& prng) {
    using std::begin, std::end;

    std::array<std::size_t, Row * Column> temp;

    for (std::size_t i = 0; i < Row; i++)
        for (std::size_t j = 0; j < Column; j++)
            temp[i * Column + j] = seats_[i][j];

    std::shuffle(begin(temp), end(temp), prng);

    for (std::size_t i = 0; i < Row; i++)
        for (std::size_t j = 0; j < Column; j++)
            seats_[i][j] = temp[i * Column + j];

    for (std::size_t i = 0; i < Row; i++)
        for (std::size_t j = 0; j < Column; j++)
            locations_[seats_[i][j]] = {i, j};
}

template<std::size_t Row, std::size_t Column>
template<typename Scorer>
bool SeatingChart<Row, Column>::hill_climb_students(Scorer& scorer) {
    double maximum_score = scorer(*this);
    bool   found_raise   = false;

    std::pair<std::size_t, std::size_t> best_swap;

    for (std::size_t i = 0; i < Row * Column; i++)
    {
        for (std::size_t j = i + 1; j < Row * Column; j++)
        {
            this->swap_students(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = std::make_pair(i, j);
            }

            this->swap_students(i, j);
        }
    }

    this->swap_students(best_swap.first, best_swap.second);

    return found_raise;
}

template<std::size_t Row, std::size_t Column>
template<typename Scorer>
bool SeatingChart<Row, Column>::hill_climb_pairs(Scorer& scorer) {
    double maximum_score = scorer(*this);
    bool   found_raise   = false;

    std::pair<std::size_t, std::size_t> best_swap;

    for (std::size_t i = 0; i < Row * Column; i++)
    {
        for (std::size_t j = i + 1; j < Row * Column; j++)
        {
            this->swap_pairs(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = std::make_pair(i, j);
            }

            this->swap_pairs(i, j);
        }
    }

    this->swap_pairs(best_swap.first, best_swap.second);

    return found_raise;
}

template<std::size_t Row, std::size_t Column>
template<typename Scorer>
bool SeatingChart<Row, Column>::hill_climb_combined(Scorer& scorer) {
    double maximum_score = scorer(*this);
    bool   found_raise   = false;

    std::tuple<std::size_t, std::size_t, bool> best_swap;

    for (std::size_t i = 0; i < Row * Column; i++)
    {
        for (std::size_t j = i + 1; j < Row * Column; j++)
        {
            swap_students(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = {i, j, false};
            }

            swap_students(i, j);
        }
    }

    for (std::size_t i = 0; i < Row * Column; i++)
    {
        for (std::size_t j = i + 1; j < Row * Column; j++)
        {
            swap_pairs(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = {i, j, true};
            }

            swap_pairs(i, j);
        }
    }

    if (found_raise)
    {
        if (std::get<2>(best_swap))
            swap_pairs(std::get<0>(best_swap), std::get<1>(best_swap));
        else
            swap_students(std::get<0>(best_swap), std::get<1>(best_swap));
    }

    return found_raise;
}

}

#endif

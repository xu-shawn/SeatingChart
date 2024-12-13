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

struct Move {
    std::size_t student1;
    std::size_t student2;
    bool        is_pair_swap;
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

    template<typename PRNG, std::size_t Swaps>
    void partial_random_shuffle(PRNG&);

    template<typename PRNG, typename PRNG::result_type probability>
    void probablistic_random_shuffle(PRNG&);

    template<typename Scorer>
    bool hill_climb_students(Scorer&);

    template<typename Scorer>
    bool hill_climb_pairs(Scorer&);

    template<typename Scorer>
    bool hill_climb_combined(Scorer&);

    template<typename Scorer>
    bool hill_climb_lookahead(Scorer&);
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
    swap_students(first, second);
    swap_students(get_tablemate(first), get_tablemate(second));
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
template<typename PRNG, std::size_t Swaps>
void SeatingChart<Row, Column>::partial_random_shuffle(PRNG& prng) {
    using distribution_type = std::uniform_int_distribution<typename PRNG::result_type>;
    static_assert(Swaps <= Row * Column);

    distribution_type gen_student{0, Row - 1};
    distribution_type coin_flip{0, 1};

    if (coin_flip(prng))
        for (std::size_t i = 0; i < Swaps; i++)
            swap_students(i, gen_student(prng));
    else
        for (std::size_t i = Row * Column - 1; i >= Row * Column - Swaps; i--)
            swap_students(i, gen_student(prng));
}

template<std::size_t Row, std::size_t Column>
template<typename PRNG, typename PRNG::result_type probability>
void SeatingChart<Row, Column>::probablistic_random_shuffle(PRNG& prng) {
    using distribution_type = std::uniform_int_distribution<typename PRNG::result_type>;
    static_assert(probability <= 1000);
    static_assert(probability > 0);

    distribution_type gen_student{0, Row - 1};
    distribution_type gen_probablistic{0, 1000};

    for (std::size_t i = 0; i < Row * Column; i++)
        if (gen_probablistic(prng) > probability)
            swap_students(i, gen_student(prng));
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
            swap_students(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = std::make_pair(i, j);
            }

            swap_students(i, j);
        }
    }

    swap_students(best_swap.first, best_swap.second);

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
            swap_pairs(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > maximum_score)
            {
                maximum_score = curr_score;
                found_raise   = true;
                best_swap     = std::make_pair(i, j);
            }

            swap_pairs(i, j);
        }
    }

    swap_pairs(best_swap.first, best_swap.second);

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

template<std::size_t Row, std::size_t Column>
template<typename Scorer>
bool SeatingChart<Row, Column>::hill_climb_lookahead(Scorer& scorer) {
    double maximum_score  = scorer(*this);
    double original_score = maximum_score;
    bool   found_raise    = false;

    std::array<Move, 2> moves;

    for (std::size_t i = 0; i < Row * Column; i++)
    {
        for (std::size_t j = i + 1; j < Row * Column; j++)
        {
            swap_students(i, j);

            const double curr_score = scorer(*this);

            if (curr_score > original_score)
            {
                for (std::size_t i2 = 0; i2 < Row * Column; i2++)
                {
                    for (std::size_t j2 = i2 + 1; j2 < Row * Column; j2++)
                    {
                        swap_students(i2, j2);

                        const double curr_score = scorer(*this);

                        if (curr_score > maximum_score)
                        {
                            maximum_score = curr_score;
                            found_raise   = true;

                            moves[0] = {i, j, false};
                            moves[1] = {i2, j2, false};
                        }

                        swap_students(i2, j2);
                    }
                }

                for (std::size_t i2 = 0; i2 < Row * Column; i2++)
                {
                    for (std::size_t j2 = i2 + 1; j2 < Row * Column; j2++)
                    {
                        swap_pairs(i2, j2);

                        const double curr_score = scorer(*this);

                        if (curr_score > maximum_score)
                        {
                            maximum_score = curr_score;
                            found_raise   = true;

                            moves[0] = {i, j, false};
                            moves[1] = {i2, j2, true};
                        }

                        swap_pairs(i2, j2);
                    }
                }
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

            if (curr_score > original_score)
            {
                for (std::size_t i2 = 0; i2 < Row * Column; i2++)
                {
                    for (std::size_t j2 = i2 + 1; j2 < Row * Column; j2++)
                    {
                        swap_students(i2, j2);

                        const double curr_score = scorer(*this);

                        if (curr_score > maximum_score)
                        {
                            maximum_score = curr_score;
                            found_raise   = true;

                            moves[0] = {i, j, true};
                            moves[1] = {i2, j2, false};
                        }

                        swap_students(i2, j2);
                    }
                }

                for (std::size_t i2 = 0; i2 < Row * Column; i2++)
                {
                    for (std::size_t j2 = i2 + 1; j2 < Row * Column; j2++)
                    {
                        swap_pairs(i2, j2);

                        const double curr_score = scorer(*this);

                        if (curr_score > maximum_score)
                        {
                            maximum_score = curr_score;
                            found_raise   = true;

                            moves[0] = {i, j, true};
                            moves[1] = {i2, j2, true};
                        }

                        swap_pairs(i2, j2);
                    }
                }
            }

            swap_pairs(i, j);
        }
    }

    for (const auto move : moves)
    {
        if (move.is_pair_swap)
            swap_pairs(move.student1, move.student2);
        else
            swap_students(move.student1, move.student2);
    }

    return found_raise;
}

}

#endif

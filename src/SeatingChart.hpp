#ifndef SEATINGCHART_HPP_INCLUDED
#define SEATINGCHART_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <random>
#include <type_traits>

#include "ClassInfo.hpp"

namespace SeatingChart {

template<std::size_t Row, std::size_t Column>
class SeatingChart {
    std::array<std::array<std::size_t, Column>, Row>    chart_;
    std::array<std::pair<size_t, size_t>, Row * Column> lookup_;

    constexpr void swap(std::size_t, std::size_t) noexcept;

   public:
    template<typename T,
             typename = typename std::enable_if_t<
               std::is_same_v<std::remove_reference_t<T>,
                              std::array<std::array<std::size_t, Column>, Row>>,
               bool>>
    constexpr SeatingChart(T&&);
    constexpr SeatingChart(const SeatingChart&) = default;
    constexpr const auto& chart() const noexcept { return chart_; }
    constexpr const auto& lookup() const noexcept { return lookup_; }

    template<typename PRNG>
    void random_shuffle(PRNG&);

    template<typename PRNG>
    void mutate(PRNG&);
};

template<std::size_t Row, std::size_t Column>
constexpr double score(const SeatingChart<Row, Column>& chart,
                       const ClassInfo<Row * Column>&   info) noexcept;

}

namespace SeatingChart {

template<std::size_t Row, std::size_t Column>
template<typename T, typename>
constexpr SeatingChart<Row, Column>::SeatingChart(T&& c) :
    chart_(std::forward<T>(c)) {
    for (size_t i = 0; i < Row; i++)
        for (size_t j = 0; j < Column; j++)
            lookup_.emplace(chart_[i][j], std::make_pair<i, j>);
}

template<std::size_t Row, std::size_t Column>
constexpr void SeatingChart<Row, Column>::swap(std::size_t first, std::size_t second) noexcept {
    using std::swap;

    assert(first >= 0 && first < Row * Column && second >= 0 && first < Row * Column);
    const auto [first_row, first_column]   = lookup_[first];
    const auto [second_row, second_column] = lookup_[second];

    swap(chart_[first_row], chart_[first_column]);
    swap(chart_[second_row], chart_[second_column]);
    swap(lookup_[first], lookup_[second]);
}

template<std::size_t Row, std::size_t Column>
template<typename PRNG>
void SeatingChart<Row, Column>::random_shuffle(PRNG& prng) {
    using std::begin, std::end;

    std::array<std::size_t, Row * Column> temp;

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            temp[i * Column + j] = chart_[i][j];

    std::shuffle(begin(temp), end(temp), prng);

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            chart_[i][j] = temp[i * Column + j];
}

template<std::size_t Row, std::size_t Column>
template<typename PRNG>
void SeatingChart<Row, Column>::mutate(PRNG& prng) {
    using distribution_type = std::uniform_int_distribution<typename PRNG::result_type>;
    static const distribution_type dist{0, Row * Column - 1};

    const int num_swaps = distribution_type{0, (Row * Column - 1) / 4}(prng);

    for (int i = 0; i < num_swaps; i++)
    {
        const int chosen_student = dist(prng);
        const auto [x, y]        = lookup_[chosen_student];

        const int destination_x =
          distribution_type{std::max(x - 2, 0), std::min(x + 2, Row - 1)}(prng);
        const int destination_y =
          distribution_type{std::max(y - 2, 0), std::min(y + 2, Row - 1)}(prng);

        this->swap(chart_[x][y], chart_[destination_x][destination_y]);
    }
}

template<std::size_t Row, std::size_t Column>
constexpr double score(const SeatingChart<Row, Column>& chart,
                       const ClassInfo<Row * Column>&   info) noexcept {
    return 0;
}

}

#endif

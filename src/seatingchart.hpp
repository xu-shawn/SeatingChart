#ifndef SEATINGCHART_HPP_INCLUDED
#define SEATINGCHART_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <random>
#include <type_traits>

namespace SeatingChart {

struct Location {
    std::size_t row;
    std::size_t column;
};

template<std::size_t Row, std::size_t Column>
class SeatingChart {
    std::array<std::array<std::size_t, Column>, Row> seats_;
    std::array<Location, Row * Column>               locations_;

    constexpr void swap(std::size_t, std::size_t) noexcept;

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

    template<typename PRNG>
    void mutate(PRNG&);
};

}

namespace SeatingChart {

template<std::size_t Row, std::size_t Column>
template<typename T, typename>
constexpr SeatingChart<Row, Column>::SeatingChart(T&& c) :
    seats_(std::forward<T>(c)) {
    for (size_t i = 0; i < Row; i++)
        for (size_t j = 0; j < Column; j++)
            locations_[seats_[i][j]] = {i, j};
}

template<std::size_t Row, std::size_t Column>
constexpr void SeatingChart<Row, Column>::swap(std::size_t first, std::size_t second) noexcept {
    using std::swap;

    assert(first >= 0 && first < Row * Column && second >= 0 && first < Row * Column);
    const auto [first_row, first_column]   = locations_[first];
    const auto [second_row, second_column] = locations_[second];

    swap(seats_[first_row], seats_[first_column]);
    swap(seats_[second_row], seats_[second_column]);
    swap(locations_[first], locations_[second]);
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

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            temp[i * Column + j] = seats_[i][j];

    std::shuffle(begin(temp), end(temp), prng);

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            seats_[i][j] = temp[i * Column + j];
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
        const auto [x, y]        = locations_[chosen_student];

        const int destination_x =
          distribution_type{std::max(x - 2, 0), std::min(x + 2, Row - 1)}(prng);
        const int destination_y =
          distribution_type{std::max(y - 2, 0), std::min(y + 2, Row - 1)}(prng);

        this->swap(seats_[x][y], seats_[destination_x][destination_y]);
    }
}

}

#endif

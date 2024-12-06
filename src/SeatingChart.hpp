#ifndef SEATINGCHART_HPP_INCLUDED
#define SEATINGCHART_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <random>
#include <unordered_map>

#include "ClassInfo.hpp"

namespace SeatingChart {

template<std::size_t Row, std::size_t Column, typename IndexType>
class SeatingChart {
    std::array<std::array<IndexType, Column>, Row>           _chart;
    std::unordered_map<IndexType, std::pair<size_t, size_t>> _lookup;

   public:
    constexpr SeatingChart(const std::array<std::array<IndexType, Row>, Column>& c);
    constexpr SeatingChart(std::array<std::array<IndexType, Row>, Column>&& c);
    constexpr SeatingChart(const SeatingChart& other) = default;
    constexpr const auto& chart() const noexcept { return _chart; }
    constexpr const auto& lookup() const noexcept { return _lookup; }

    template<typename PRNG>
    friend void random_shuffle(SeatingChart<Row, Column, IndexType>& chart, PRNG& prng);
};

template<std::size_t Row, std::size_t Column, typename IndexType, typename PRNG>
void random_shuffle(SeatingChart<Row, Column, IndexType>& chart, PRNG& prng);

template<std::size_t Row, std::size_t Column, typename IndexType, typename PRNG>
void mutate(SeatingChart<Row, Column, IndexType>& chart, PRNG& prng);

template<std::size_t Row, std::size_t Column, typename IndexType>
constexpr double score(const SeatingChart<Row, Column, IndexType>& chart, const ClassInfo& info);

}

namespace SeatingChart {

template<std::size_t Row, std::size_t Column, typename IndexType>
constexpr SeatingChart<Row, Column, IndexType>::SeatingChart(
  const std::array<std::array<IndexType, Row>, Column>& c) :
    _chart(c) {
    for (size_t i = 0; i < Row; i++)
    {
        for (size_t j = 0; j < Column; j++)
        {
            _lookup.emplace(_chart[i][j], std::make_pair<i, j>);
        }
    }
}

template<std::size_t Row, std::size_t Column, typename IndexType>
constexpr SeatingChart<Row, Column, IndexType>::SeatingChart(
  std::array<std::array<IndexType, Row>, Column>&& c) :
    _chart(std::move(c)) {
    for (size_t i = 0; i < Row; i++)
    {
        for (size_t j = 0; j < Column; j++)
        {
            _lookup.emplace(_chart[i][j], std::make_pair<i, j>);
        }
    }
}

template<std::size_t Row, std::size_t Column, typename IndexType, typename PRNG>
void random_shuffle(SeatingChart<Row, Column, IndexType>& chart, PRNG& prng) {
    using std::begin, std::end;

    std::array<IndexType, Row * Column> temp;

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            temp[i * Column + j] = chart.chart()[i][j];

    std::shuffle(begin(temp), end(temp), prng);

    for (int i = 0; i < Row; i++)
        for (int j = 0; j < Column; j++)
            chart.chart[i][j] = temp[i * Column + j];
}

template<std::size_t Row, std::size_t Column, typename IndexType, typename PRNG>
void mutate(SeatingChart<Row, Column, IndexType>& chart, PRNG& prng);

template<std::size_t Row, std::size_t Column, typename IndexType>
constexpr double score(const SeatingChart<Row, Column, IndexType>& chart, const ClassInfo& info);

}

#endif

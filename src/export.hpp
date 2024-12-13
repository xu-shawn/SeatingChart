#ifndef EXPORT_HPP_INCLUDED
#define EXPORT_HPP_INCLUDED

#include <fstream>
#include <string>

#include "seatingchart.hpp"

namespace SeatingChartGenetic {

template<std::size_t Row, std::size_t Column>
void export_chart(const SeatingChart<Row, Column>&,
                  const std::array<std::string, Row * Column>&,
                  std::ofstream&&);

}

namespace SeatingChartGenetic {

template<std::size_t Row, std::size_t Column>
void export_chart(const SeatingChart<Row, Column>&             chart,
                  const std::array<std::string, Row * Column>& names,
                  std::ofstream&&                              file) {
    for (const auto& row : chart.seats())
        for (const auto element : row)
            file << names[element] << std::endl;
}

}

#endif

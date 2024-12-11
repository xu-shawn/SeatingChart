#ifndef PARSE_HPP_INCLUDED
#define PARSE_HPP_INCLUDED

#include <cassert>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>

#include "classinfo.hpp"
#include "seatingchart.hpp"
#include "utils.hpp"

namespace SeatingChart {

template<std::size_t Row, std::size_t Column>
struct ParseResult {
    std::array<std::string, Row * Column> lookup_name;
    SeatingChart<Row, Column>             chart;
    ClassInfo<Row * Column>               class_info;
};

template<std::size_t Row, std::size_t Column>
ParseResult<Row, Column> parse(std::ifstream&& input) {
    int row, column;
    input >> row >> column;
    assert(row == Row && column == Column);

    std::array<std::string, Row * Column>            lookup;
    std::array<std::array<std::size_t, Column>, Row> seats;

    for (int i = 0; i < Row; i++)
    {
        for (int j = 0; j < Column; j++)
        {
            input >> lookup[i * Column + j];
            seats[i][j] = i * Column + j;
        }
    }

    std::string                                line;
    std::array<std::vector<int>, Row * Column> friends;
    std::array<std::vector<int>, Row * Column> enemies;

    for (int i = 0; i < Row * Column; i++)
    {
        std::getline(input, line);

        if (line.empty())
        {
            i--;
            continue;
        }

        const auto  colon_idx = line.find(':');
        const auto  key       = line.substr(0, colon_idx);
        std::size_t index_key;

        for (std::size_t j = 0; j < size(lookup); j++)
        {
            if (key == lookup[j])
            {
                index_key = j;
                break;
            }
        }

        const auto str_data = utils::split(line.substr(colon_idx + 2), ' ');

        for (const auto& person : str_data)
            friends[index_key].push_back(
              std::distance(lookup.begin(), std::find(lookup.begin(), lookup.end(), person)));
    }

    for (int i = 0; i < Row * Column; i++)
    {
        std::getline(input, line);

        if (line.empty())
        {
            i--;
            continue;
        }

        const auto  colon_idx = line.find(':');
        const auto  key       = line.substr(0, colon_idx);
        std::size_t index_key;

        for (std::size_t j = 0; j < size(lookup); j++)
        {
            if (key == lookup[j])
            {
                index_key = j;
                break;
            }
        }

        const auto str_data = utils::split(line.substr(colon_idx + 2), ' ');

        for (const auto& person : str_data)
            enemies[index_key].push_back(
              std::distance(lookup.begin(), std::find(lookup.begin(), lookup.end(), person)));
    }

    return ParseResult<Row, Column>{
      lookup, SeatingChart<Row, Column>{std::move(seats)},
      ClassInfo<Row * Column>{std::move(friends), std::move(enemies)}};
}

}


#endif

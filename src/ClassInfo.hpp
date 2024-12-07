#ifndef CLASSINFO_HPP_INCLUDED
#define CLASSINFO_HPP_INCLUDED

#include <unordered_set>
#include <vector>

namespace SeatingChart {

struct ClassInfo {
    std::vector<std::vector<int>>  friends;
    std::vector<std::vector<int>>  enemies;
    std::vector<std::vector<bool>> friends_lookup;
    std::vector<std::vector<bool>> enemies_lookup;
};

}

#endif

#ifndef CLASSINFO_HPP_INCLUDED
#define CLASSINFO_HPP_INCLUDED

#include <unordered_set>
#include <vector>

namespace SeatingChart {

struct ClassInfo {
    std::vector<std::unordered_set<int>> friends;
    std::vector<std::unordered_set<int>> enemies;
};

}

#endif

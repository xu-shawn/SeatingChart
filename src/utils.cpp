#include "utils.hpp"

#include <string>
#include <sstream>
#include <vector>

namespace SeatingChartTSP::utils {

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result{};

    std::istringstream stream{str};

    for (std::string token{}; std::getline(stream, token, delim);)
    {
        if (token.empty())
            continue;

        result.push_back(token);
    }

    return result;
}

}

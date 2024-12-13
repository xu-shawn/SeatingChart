#ifndef CLASSINFO_HPP_INCLUDED
#define CLASSINFO_HPP_INCLUDED

#include <array>
#include <vector>

namespace SeatingChart {

template<size_t NumStudents>
class ClassInfo {
    std::array<std::vector<int>, NumStudents>              friends;
    std::array<std::vector<int>, NumStudents>              enemies;
    std::array<std::array<bool, NumStudents>, NumStudents> friends_lookup;
    std::array<std::array<bool, NumStudents>, NumStudents> enemies_lookup;

   public:
    template<
      typename T,
      typename U,
      typename = typename std::enable_if_t<
        std::is_same_v<std::remove_reference_t<T>, std::array<std::vector<int>, NumStudents>>,
        bool>,
      typename = typename std::enable_if_t<
        std::is_same_v<std::remove_reference_t<U>, std::array<std::vector<int>, NumStudents>>,
        bool>>
    constexpr ClassInfo(T&&, U&&);
    constexpr ClassInfo(const ClassInfo&) = default;

    [[nodiscard]] constexpr bool friends_towards(std::size_t, std::size_t) const noexcept;
    [[nodiscard]] constexpr bool enemies_towards(std::size_t, std::size_t) const noexcept;

    [[nodiscard]] constexpr const auto& enemies_of(std::size_t) const noexcept;
    [[nodiscard]] constexpr const auto& friends_of(std::size_t) const noexcept;
};

}

namespace SeatingChart {

template<size_t NumStudents>
template<typename T, typename U, typename, typename>
constexpr ClassInfo<NumStudents>::ClassInfo(T&& f, U&& s) :
    friends{std::forward<T>(f)},
    enemies{std::forward<U>(s)},
    friends_lookup{{{false}}},
    enemies_lookup{{{false}}} {
    using std::size;

    for (std::size_t student = 0; student < size(friends); student++)
        for (const auto stu_friend : friends[student])
            friends_lookup[student][stu_friend] = true;

    for (std::size_t student = 0; student < size(enemies); student++)
        for (const auto stu_enemy : enemies[student])
            enemies_lookup[student][stu_enemy] = true;
}

template<size_t NumStudents>
constexpr bool ClassInfo<NumStudents>::friends_towards(std::size_t stu_from,
                                                       std::size_t stu_to) const noexcept {
    return friends_lookup[stu_from][stu_to];
}

template<size_t NumStudents>
constexpr bool ClassInfo<NumStudents>::enemies_towards(std::size_t stu_from,
                                                       std::size_t stu_to) const noexcept {
    return enemies_lookup[stu_from][stu_to];
}

template<size_t NumStudents>
constexpr const auto& ClassInfo<NumStudents>::friends_of(std::size_t student) const noexcept {
    return friends[student];
}

template<size_t NumStudents>
constexpr const auto& ClassInfo<NumStudents>::enemies_of(std::size_t student) const noexcept {
    return enemies[student];
}


}

#endif

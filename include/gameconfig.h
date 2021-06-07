#ifndef GAMECONFIG_H
#define GAMECONFIG_H

template<BoardConfig config>
struct GameConfig;

template<>
struct GameConfig<BoardConfig::Easy> {
    static constexpr unsigned int rows = 4;
    static constexpr unsigned int cols = 3;
    static constexpr unsigned int ressize = 7;
};

template<>
struct GameConfig<BoardConfig::Medium> {
    static constexpr unsigned int rows = 5;
    static constexpr unsigned int cols = 6;
    static constexpr unsigned int ressize = 15;
};

#endif
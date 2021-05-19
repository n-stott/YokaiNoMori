#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <string>
#include <sstream>

#define USE_LOGGER 0

enum class Verb {
    None,
    Std,
    Dev
};

struct Logger {

    static constexpr Verb verbosity = Verb::Dev;

    template<typename Message>
    static void log(Verb v, Message message) {
        if(static_cast<int>(v) <= static_cast<int>(verbosity)) {
            const std::string s = message();
            std::puts(s.c_str());
        }
    }

};

#endif
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

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
            std::string s = message();
            std::cout << s << std::endl;
        }
    }

};

#endif
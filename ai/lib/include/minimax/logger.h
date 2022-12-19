#ifndef LOGGER_H
#define LOGGER_H

#include <fmt/core.h>

#define USE_LOGGER 0

enum class Verb {
    None,
    Std,
    Dev
};

struct Logger {

    static constexpr Verb verbosity = Verb::Std;

    template<typename WriteMessage>
    static void with(Verb v, WriteMessage writeMessage) {
        if(static_cast<int>(v) <= static_cast<int>(verbosity)) {
            writeMessage();
        }
    }

};

#endif
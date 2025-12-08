#pragma once

#include <string>
#include <iostream>

namespace fenrir {
class Logger {
public:
    Logger() = default;
    ~Logger() = default;

    inline void log(const std::string& message){
        std::cout << message << std::endl;
    }
};
}  // namespace fenrir

#pragma once

#include <string>

class InputManager {
public:
    InputManager();
    ~InputManager();

    std::string getCommand();
};

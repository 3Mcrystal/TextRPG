#include <iostream>

#include "InputManager.h"

InputManager::InputManager() {}
InputManager::~InputManager() {}

std::string InputManager::getCommand() {
    std::string cmd;
    std::cout << "> ";
    std::getline(std::cin, cmd);
    return cmd;
}

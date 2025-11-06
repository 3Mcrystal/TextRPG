#pragma once

#include <string>

class InputManager {
public:
    InputManager();
    ~InputManager();

    std::string GetLine();

	std::string RequestActionFor(const std::string& characterName);

	int RequestTragetIndex(int maxIndex);
};

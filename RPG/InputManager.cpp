#include <iostream>

#include "InputManager.h"

InputManager::InputManager() {}
InputManager::~InputManager() {}

std::string InputManager::GetLine() {
	std::string s;
	std::getline(std::cin, s);
	return s;
}

std::string InputManager::RequestActionFor(const std::string& characterName) {
	std::cout << "Enter action for " << characterName << " ( attack / defend / skill / run / status / item)\n> ";
	std::string cmd = GetLine();
	return cmd;
}

int InputManager::RequestTragetIndex(int maxIndex) {
	std::cout << "Enter target index (0 - " << maxIndex - 1 << "):\n> ";
	std::string s = GetLine();
	try {
		int v = std::stoi(s);
		if (v < 0) v = 0;
		if (v >= maxIndex) v = maxIndex - 1;
		return v;
	}
	catch (...) { // catch all exceptions
		return 0;
	}
}
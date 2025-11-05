#include <iostream>

#include "GameManager.h"
#include "InputManager.h"
#include "TurnManager.h"

GameManager::GameManager()
	: m_running(false) {
}

GameManager::~GameManager() {}

void GameManager::start() {
	m_input = std::make_unique<InputManager>();
	m_turns = std::make_unique<TurnManager>();

	//TODO: create player / enemies -> TurnManager

	m_running = true;
	std::cout << "RPG" << std::endl;
}

void GameManager::loop() {

//OUTDATED : TO UODATE
//	while (m_running) {
//		std::cout << "Enter a command (n/s/e/w, status, quit): \n";
//		std::string cmd = m_input->getCommand();
//
//		if (cmd == "quit" || cmd == "q") {
//			m_running = false;
//			continue;
//		}
//		else if (cmd == "status") {
//			std::cout << "OK "; //TODO
//			m_turns->StartTurnCycle();
//
//		}
//		//TODO WIN/LOSE conditions
//	}
}

void GameManager::shutdown() {
	std::cout << "Shutting down...\n";
}
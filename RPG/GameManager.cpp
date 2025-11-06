#include <iostream>
#include <memory>
#include <random>

#include "GameManager.h"
#include "InputManager.h"
#include "TurnManager.h"
#include "PlayerParty.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Encounter.h"
#include "Goblin.h"
#include "Orc.h"
#include "Skeleton.h"
#include "Beggar.h"

GameManager::GameManager() : m_running(false) {}
GameManager::~GameManager() {}

void GameManager::start() {
	m_input = std::make_unique<InputManager>();
	m_turns = std::make_unique<TurnManager>();
	m_party = std::make_unique<PlayerParty>();

	//create  party
	m_party->AddMember(std::make_shared<Warrior>("W"));
	m_party->AddMember(std::make_shared<Mage>("M"));
	m_party->AddMember(std::make_shared<Ranger>("R"));

	m_running = true;
	std::cout << "Game started!\n";
	std::cout << "Use explore to find encounters.\n";
	std::cout << "Use status to check party status.\n";
	std::cout << "Use quit to exit the game.\n";
}


void GameManager::loop() {
	while (m_running) {
		std::cout << "\n";
		std::string cmd = m_input->GetLine();
		if (cmd == "explore" || cmd == "e") {
			auto encounter = GenerateRandomEncounter();
			std::cout << "An encounter has begun!\n";

			if (encounter->IsBeggarEncounter()) {
				std::cout << "it's a beggar\n";
			}

			else
			{
				std::cout << "Enemies:\n";
				int i = 0;
				for (auto& e : encounter->GetEnemies()) {
					std::cout << "[" << i++ << "] " << e->GetName() << " (HP: " << e->GetHp() << "\n";
				}
			}

			bool survived = m_turns->ExecuteEncounter(*m_party, *encounter, *m_input);
			if (!survived) {
				std::cout << "Game Over!\n";
				m_running = false;
			}
			else {
				std::cout << "You survived the encounter!\n";
			}
		}
		else if (cmd == "status" || cmd == "s") {
			m_party->PrintStatus();
		}
		else if (cmd == "quit" || cmd == "q") {
			std::cout << "Quitting game...\n";
			m_running = false;
		}
		else {
			std::cout << "Unknown command.\n";
			std::cout << "Commands: explore (e), status (s), quit (q)\n";
		}
	}
}

void GameManager::shutdown() {
	std::cout << "Shutting down...\n";
}



//Generate random encounter
std::unique_ptr<Encounter> GameManager::GenerateRandomEncounter() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> EncounterType(0, 5); //0-4 fight, 5 beggar (20%)
	int t = EncounterType(gen);

	auto encounter = std::make_unique<Encounter>();
	if (t == 5) {
		//Beggar encounter
		encounter->SetBeggarEncounter(true);
		encounter->AddEnemy(std::make_shared<Beggar>("Beggar"));
		return encounter;
	}

	//Combat encounter
	std::uniform_int_distribution<> EnemyCountDist(1, 3); //1-3 enemies
	int enemyCount = EnemyCountDist(gen);

	for (int i = 0; i < enemyCount; ++i) {
		std::uniform_int_distribution<> EnemyTypeDist(0, 2); //0-Goblin,1-Orc,2-Skeleton
		int et = EnemyTypeDist(gen);
		if (et == 0) {
			encounter->AddEnemy(std::make_shared<Goblin>("Goblin"));
		}
		else if (et == 1) {
			encounter->AddEnemy(std::make_shared<Orc>("Orc"));
		}
		else {
			encounter->AddEnemy(std::make_shared<Skeleton>("Skeleton"));
		}
	}
	return encounter;
}
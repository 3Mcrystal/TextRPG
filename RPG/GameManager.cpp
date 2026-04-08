#include <iostream>
#include <memory>
#include <random>
#include <algorithm>

#include "GameManager.h"
#include "InputManager.h"
#include "TurnManager.h"
#include "PlayerParty.h"
#include "Character.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Encounter.h"
#include "Goblin.h"
#include "Orc.h"
#include "Skeleton.h"
#include "Beggar.h"
#include "Troll.h"
#include "AsciiArt.h"

GameManager::GameManager() : m_running(false) {}
GameManager::~GameManager() {}

// ─────────────────────────────────────────────────────────────
void GameManager::start()
{
	m_input = std::make_unique<InputManager>();
	m_turns = std::make_unique<TurnManager>();
	m_party = std::make_unique<PlayerParty>();

	m_party->AddMember(std::make_shared<Warrior>("Slankec"));
	m_party->AddMember(std::make_shared<Mage>("Narchis"));
	m_party->AddMember(std::make_shared<Ranger>("Aethyra"));

	m_running = true;

	AsciiArt::PrintTitle();
	AsciiArt::PrintDivider();
	AsciiArt::PrintWarrior();
	AsciiArt::PrintMage();
	AsciiArt::PrintRanger();
	AsciiArt::PrintDivider();
	std::cout << "Your party is ready for adventure!\n\n";
	std::cout << "Commands: explore (e)  |  rest (r)  |  status (s)  |  quit (q)\n";
}

// ─────────────────────────────────────────────────────────────
void GameManager::loop()
{
	while (m_running) {
		std::cout << "\n> ";
		std::string cmd = m_input->GetLine();

		// ── Explore ──────────────────────────────────────────
		if (cmd == "explore" || cmd == "e")
		{
			// Boss every 5 combat encounters
			bool isBoss = (m_fightCount > 0 && m_fightCount % 5 == 0);
			auto encounter = isBoss ? GenerateBossEncounter()
			                        : GenerateRandomEncounter();

			// Show enemy list for combat encounters
			if (!encounter->IsBeggarEncounter() && !encounter->IsMerchantEncounter()) {
				std::cout << "\n";
				int i = 0;
				for (auto& e : encounter->GetEnemies())
					std::cout << "[" << i++ << "] " << e->GetName()
					          << "  HP: " << e->GetHp() << "\n";
			}

			bool survived = m_turns->ExecuteEncounter(*m_party, *encounter, *m_input);

			if (!survived) {
				m_running = false;
				continue;
			}

			// Count combat wins (not beggar/merchant)
			if (!encounter->IsBeggarEncounter() && !encounter->IsMerchantEncounter()) {
				m_fightCount++;
				if (m_fightCount % 5 == 0)
					std::cout << "Next encounter will be a BOSS!\n";
			}

			// Unlock rest after each encounter
			m_canRest = true;

			std::cout << "You survived the encounter!\n";
			std::cout << "Commands: explore (e)  |  rest (r)  |  status (s)  |  quit (q)\n";
		}

		// ── Rest ─────────────────────────────────────────────
		else if (cmd == "rest" || cmd == "r")
		{
			DoRest();
		}

		// ── Status ───────────────────────────────────────────
		else if (cmd == "status" || cmd == "s")
		{
			m_party->PrintStatus();
		}

		// ── Quit ─────────────────────────────────────────────
		else if (cmd == "quit" || cmd == "q")
		{
			std::cout << "Quitting game...\n";
			m_running = false;
		}
		else
		{
			std::cout << "Unknown command.\n";
			std::cout << "Commands: explore (e)  |  rest (r)  |  status (s)  |  quit (q)\n";
		}
	}
}

// ─────────────────────────────────────────────────────────────
void GameManager::shutdown()
{
	std::cout << "Shutting down...\n";
}

// ─────────────────────────────────────────────────────────────
void GameManager::DoRest()
{
	if (!m_canRest) {
		std::cout << "Your party needs to explore before resting again.\n";
		return;
	}

	AsciiArt::PrintCampfire();

	const int HEAL_AMOUNT = 15;
	bool anyHealed = false;

	for (auto& m : m_party->GetMembers()) {
		if (!m->IsAlive()) continue;
		int before = m->GetHp();
		int after  = std::min(before + HEAL_AMOUNT, m->GetMaxHp());
		m->SetHp(after);
		int gained = after - before;
		if (gained > 0) {
			std::cout << m->GetName() << " rests and recovers " << gained << " HP.\n";
			anyHealed = true;
		} else {
			std::cout << m->GetName() << " is already at full HP.\n";
		}
	}

	if (anyHealed)
		std::cout << "The party feels refreshed.\n";

	m_canRest = false; // must explore before resting again
}

// ─────────────────────────────────────────────────────────────
std::unique_ptr<Encounter> GameManager::GenerateBossEncounter()
{
	auto encounter = std::make_unique<Encounter>();
	encounter->SetBossEncounter(true);
	encounter->AddEnemy(std::make_shared<Troll>("Troll"));
	return encounter;
}

// ─────────────────────────────────────────────────────────────
std::unique_ptr<Encounter> GameManager::GenerateRandomEncounter()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> roll(0, 99);
	int t = roll(gen);

	auto encounter = std::make_unique<Encounter>();

	// 70% combat | 15% beggar | 15% merchant
	if (t >= 70 && t <= 84) {
		encounter->SetBeggarEncounter(true);
		encounter->AddEnemy(std::make_shared<Beggar>("Beggar"));
		return encounter;
	}
	if (t >= 85) {
		encounter->SetMerchantEncounter(true);
		return encounter;
	}

	// Combat: 1–3 enemies
	std::uniform_int_distribution<> countDist(1, 3);
	int count = countDist(gen);
	for (int i = 0; i < count; ++i) {
		std::uniform_int_distribution<> typeDist(0, 2);
		switch (typeDist(gen)) {
			case 0: encounter->AddEnemy(std::make_shared<Goblin>("Goblin"));   break;
			case 1: encounter->AddEnemy(std::make_shared<Orc>("Orc"));         break;
			default: encounter->AddEnemy(std::make_shared<Skeleton>("Skeleton")); break;
		}
	}
	return encounter;
}

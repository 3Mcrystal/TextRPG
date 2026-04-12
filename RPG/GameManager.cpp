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
#include "Troll.h"
#include "Dragon.h"
#include "Beggar.h"
#include "AsciiArt.h"

static const int FINAL_BOSS_THRESHOLD = 10; //fights before Dragon unlocks
static const int REST_COST			  = 5;  //Gold per rest (it's not logic, yes)
static const int REST_HP              = 25; //fix
static const int REST_MP_PCT		  = 40; //percentage of max

GameManager::GameManager() {}
GameManager::~GameManager() {}

void GameManager::start()
{
	m_input  = std::make_unique<InputManager>();
	m_turns  = std::make_unique<TurnManager>();
	m_party  = std::make_unique<PlayerParty>();
	m_world  = std::make_unique<WorldMap>();
	m_quests = std::make_unique<QuestLog>();

	m_party->AddMember(std::make_shared<Warrior>("Slankec"));
	m_party->AddMember(std::make_shared<Mage>("Narchis"));
	m_party->AddMember(std::make_shared<Ranger>("Aethyra"));

	m_running = true;

	AsciiArt::PrintTitle();
	std::cout << R"(
  The kingdom of Ashveil is under siege.
  Ignarax the Dragon descends from the Shattered Peaks,
  burning everything in its path.

  Three heroes answer the call.
  They must grow strong enough to face the Dragon
  before there is nothing left to save.

)";
	AsciiArt::PrintDivider();
	AsciiArt::PrintWarrior();
	AsciiArt::PrintMage();
	AsciiArt::PrintRanger();
	AsciiArt::PrintDivider();

	std::cout << "Your party stands at the gates of Ashveil.\n\n";
	m_world->PrintLocationArt();
	std::cout << m_world->GetCurrent().flavorText << "\n\n";
	PrintHelp();
}

// ─────────────────────────────────────────────────────────────
void GameManager::loop()
{
	while (m_running) {
		//Check win
		if (m_gameWon) {
			std::cout << "Thanks for playing! (quit to exit)\n";
			std::string q; std::getline(std::cin, q);
			m_running = false;
			break;
		}

		const auto& loc = m_world->GetCurrent();
		std::cout << "\n[" << loc.name << "]  Gold: " << m_party->GetGold()
		          << "  Fights: " << m_fightCount;
		if (m_finalUnlocked)
			std::cout << "  ** DRAGON AWAITS in the Mountains **";
		std::cout << "\n> ";

		std::string cmd = m_input->GetLine();

		//explore
		if (cmd == "explore" || cmd == "e") {
			if (loc.isSafe) {
				std::cout << "Ashveil is safe — travel to another location to explore.\n";
				continue;
			}

			auto encounter = GenerateEncounter();

			//Show enemy list for combat
			if (!encounter->IsBeggarEncounter() && !encounter->IsMerchantEncounter()) {
				std::cout << "\n";
				int i = 0;
				for (auto& e : encounter->GetEnemies())
					std::cout << "[" << i++ << "] " << e->GetName()
					          << "  HP: " << e->GetHp() << "\n";
			}

			bool survived = m_turns->ExecuteEncounter(
				*m_party, *encounter, *m_input, m_quests.get());

			if (!survived) { m_running = false; continue; }

			//Final boss win condition
			if (encounter->IsFinalBoss()) {
				m_gameWon = true;
				CollectQuestRewards();
				continue;
			}

			//Count wins
			if (!encounter->IsBeggarEncounter() && !encounter->IsMerchantEncounter()) {
				m_fightCount++;
				m_quests->OnFightSurvived();
				CollectQuestRewards();

				if (m_fightCount >= FINAL_BOSS_THRESHOLD && !m_finalUnlocked) {
					m_finalUnlocked = true;
					AsciiArt::PrintDivider();
					std::cout << "  *** The ground trembles. ***\n";
					std::cout << "  *** Ignarax has been spotted above the Shattered Peaks. ***\n";
					std::cout << "  *** Travel to the Mountains to face the Dragon. ***\n";
					AsciiArt::PrintDivider();
				}
			}

			m_canRest = true;
			std::cout << "You survived!\n";
			PrintHelp();
		}

		//Travel
		else if (cmd == "travel" || cmd == "t") {
			m_world->PrintMap();
			std::cout << "Travel to (0-3, or 'back'): ";
			std::string s = m_input->GetLine();
			int choice = -1;
			try { choice = std::stoi(s); } catch (...) {}
			if (choice >= 0)
				m_world->Travel(choice);
		}

		//Town menu
		else if (cmd == "town" || cmd == "w") {
			if (m_world->CurrentId() != LocationId::Town) {
				std::cout << "You're not in town. Use 'travel' to return to Ashveil.\n";
			} else {
				DoTownMenu();
			}
		}

		//Rest
		else if (cmd == "rest" || cmd == "r") {
			if (m_world->CurrentId() == LocationId::Town) {
				DoRest(true); // free in town
			} else {
				DoRest(false); // costs gold in the field
			}
		}

		//Inventory
		else if (cmd == "inventory" || cmd == "i") {
			DoInventory();
		}

		//Quests
		else if (cmd == "quests" || cmd == "q") {
			m_quests->PrintBoard();
		}

		//Status
		else if (cmd == "status" || cmd == "s") {
			m_party->PrintStatus();
		}

		//Map
		else if (cmd == "map" || cmd == "m") {
			m_world->PrintMap();
		}

		//Quit
		else if (cmd == "quit") {
			std::cout << "Quitting...\n";
			m_running = false;
		}
		else {
			std::cout << "Unknown command.\n";
			PrintHelp();
		}
	}
}

void GameManager::shutdown() {
	std::cout << "Shutting down...\n";
}

void GameManager::DoTownMenu()
{
	AsciiArt::PrintDivider();
	std::cout << "  ASHVEIL — Town\n";
	AsciiArt::PrintThinDivider();
	std::cout << " r) Rest       (free)\n";
	std::cout << " i) Inventory\n";
	std::cout << " q) Notice Board (quests)\n";
	std::cout << " s) Status\n";
	std::cout << " shop) Visit Merchant\n";
	AsciiArt::PrintDivider();
	std::cout << "> ";

	std::string choice = m_input->GetLine();
	if (choice == "r" || choice == "rest")       DoRest(true);
	else if (choice == "i" || choice == "inventory") DoInventory();
	else if (choice == "q" || choice == "quests")    m_quests->PrintBoard();
	else if (choice == "s" || choice == "status")    m_party->PrintStatus();
	else if (choice == "shop") {
		auto enc = std::make_unique<Encounter>();
		enc->SetMerchantEncounter(true);
		m_turns->ExecuteEncounter(*m_party, *enc, *m_input, nullptr);
	}
	else std::cout << "Unknown town command.\n";
}

void GameManager::DoRest(bool free)
{
	if (!m_canRest) {
		std::cout << "Your party needs to do something before resting again.\n";
		return;
	}

	if (!free) {
		if (m_party->GetGold() < REST_COST) {
			std::cout << "Not enough gold to make camp. (" << REST_COST << " gold needed)\n";
			return;
		}
		m_party->AddGold(-REST_COST);
		std::cout << "Spent " << REST_COST << " gold on supplies.\n";
	}

	AsciiArt::PrintCampfire();

	for (auto& m : m_party->GetMembers()) {
		if (!m->IsAlive()) continue;
		int hpBefore = m->GetHp();
		int newHp    = std::min(hpBefore + REST_HP, m->GetMaxHp());
		m->SetHp(newHp);
		int hpGained = newHp - hpBefore;

		//Restore MP
		int mpBefore = m->GetMp();
		int mpRestore = m->GetMaxMp() * REST_MP_PCT / 100;
		m->RestoreMp(mpRestore);
		int mpGained = m->GetMp() - mpBefore;

		std::cout << m->GetName() << " rests";
		if (hpGained > 0) std::cout << " [+" << hpGained << " HP]";
		else               std::cout << " [HP full]";
		if (mpGained > 0)  std::cout << " [+" << mpGained << " MP]";
		std::cout << "\n";
	}

	m_canRest = false;
}

void GameManager::DoInventory() const
{
	AsciiArt::PrintDivider();
	std::cout << "  INVENTORY  |  Gold: " << m_party->GetGold() << "\n";
	AsciiArt::PrintThinDivider();
	m_party->GetInventory().PrintInventory();
	AsciiArt::PrintDivider();
}

void GameManager::PrintHelp() const
{
	const auto& loc = m_world->GetCurrent();
	std::cout << "Commands: ";
	if (!loc.isSafe) std::cout << "explore (e)  ";
	std::cout << "travel (t)  rest (r)  status (s)  inventory (i)  quests (q)  map (m)";
	if (loc.isSafe) std::cout << "  town (w)";
	std::cout << "  quit\n";
}

void GameManager::CollectQuestRewards()
{
	auto rewards = m_quests->CollectCompletedRewards();
	for (auto& r : rewards) {
		AsciiArt::PrintThinDivider();
		std::cout << "  QUEST COMPLETE: " << r.questName << "!\n";
		std::cout << "  Reward: +" << r.xp << " XP";
		if (r.gold > 0) std::cout << " + " << r.gold << " Gold";
		std::cout << "\n";
		AsciiArt::PrintThinDivider();
		m_party->DistributeXp(r.xp);
		if (r.gold > 0) m_party->AddGold(r.gold);
	}
}

std::unique_ptr<Encounter> GameManager::GenerateEncounter()
{
	const auto& loc = m_world->GetCurrent();

	if (loc.id == LocationId::Mountains && m_finalUnlocked) {
		// 30% trigger final boss
		static std::random_device rd2; static std::mt19937 gen2(rd2());
		std::uniform_int_distribution<> trigger(1, 100);
		if (trigger(gen2) <= 30)
			return GenerateFinalBoss();
	}

	//Boss every 5 fights
	if (m_fightCount > 0 && m_fightCount % 5 == 0) {
		// Only trigger once per threshold
		static int lastBossFight = -1;
		if (lastBossFight != m_fightCount) {
			lastBossFight = m_fightCount;
			return GenerateBossEncounter(m_fightCount);
		}
	}

	static std::random_device rd; static std::mt19937 gen(rd());
	std::uniform_int_distribution<> roll(0, 99);
	int t = roll(gen);

	auto enc = std::make_unique<Encounter>();

	// 70% combat | 10% beggar | 15% merchant
	if (t >= 85) { enc->SetMerchantEncounter(true); return enc; }
	if (t >= 75) {
		enc->SetBeggarEncounter(true);
		enc->AddEnemy(std::make_shared<Beggar>("Beggar"));
		return enc;
	}

	return GenerateCombatEncounter(m_fightCount);
}

std::unique_ptr<Encounter> GameManager::GenerateCombatEncounter(int scalingLevel)
{
	const auto& loc = m_world->GetCurrent();
	static std::random_device rd; static std::mt19937 gen(rd());

	auto enc = std::make_unique<Encounter>();

	std::uniform_int_distribution<> countDist(1, 3);
	int count = countDist(gen);

	//Scaling: 3 fights -> +2 HP, +1 ATK for enemies
	int hpBonus  = (scalingLevel / 3) * 2;
	int atkBonus = (scalingLevel / 3) * 1;

	//Weighted table from location
	int total = loc.goblinWeight + loc.orcWeight + loc.skeletonWeight;
	if (total == 0) total = 1; //Safe zone, town only

	for (int i = 0; i < count; ++i) {
		std::uniform_int_distribution<> typeDist(0, total - 1);
		int roll = typeDist(gen);
		std::shared_ptr<Character> enemy;

		if (roll < loc.goblinWeight) {
			auto g = std::make_shared<Goblin>("Goblin");
			g->SetHp(g->GetHp() + hpBonus);
			enemy = g;
		} else if (roll < loc.goblinWeight + loc.orcWeight) {
			auto o = std::make_shared<Orc>("Orc");
			o->SetHp(o->GetHp() + hpBonus);
			enemy = o;
		} else {
			auto s = std::make_shared<Skeleton>("Skeleton");
			s->SetHp(s->GetHp() + hpBonus);
			enemy = s;
		}
		(void)atkBonus;
		enc->AddEnemy(enemy);
	}
	return enc;
}

std::unique_ptr<Encounter> GameManager::GenerateBossEncounter(int scalingLevel)
{
	auto enc = std::make_unique<Encounter>();
	enc->SetBossEncounter(true);
	auto troll = std::make_shared<Troll>("Troll");
	//Scale boss HP too
	int hpBonus = (scalingLevel / 3) * 5;
	troll->SetHp(troll->GetHp() + hpBonus);
	enc->AddEnemy(troll);
	return enc;
}

std::unique_ptr<Encounter> GameManager::GenerateFinalBoss()
{
	auto enc = std::make_unique<Encounter>();
	enc->SetBossEncounter(true);
	enc->SetFinalBoss(true);
	enc->AddEnemy(std::make_shared<Dragon>());
	return enc;
}

void GameManager::RollLocationEvent()
{
    static std::random_device rd; static std::mt19937 gen(rd());
    std::uniform_int_distribution<> roll(1, 100);
    int r = roll(gen);
    if (r > 20) return; // 20% chance event

    const auto& loc = m_world->GetCurrent();
    AsciiArt::PrintThinDivider();

    switch (loc.id) {
        case LocationId::Forest:
            // Mysterious shrine: buff random ALIVE member
            {
                auto alive = m_party->GetAliveMembers();
                if (alive.empty()) break;
                std::uniform_int_distribution<> idx(0,(int)alive.size()-1);
                auto& member = alive[idx(gen)];
                int heal = 10;
                member->SetHp(std::min(member->GetHp() + heal, member->GetMaxHp()));
                std::cout << "  A moss-covered shrine glows faintly in the dark.\n";
                std::cout << "  " << member->GetName() << " feels its warmth. (+" << heal << " HP)\n";
            }
            break;
        case LocationId::Dungeon:
            // Trapped chest: 40% gold, 50% damage, 10% curse
            {
                std::uniform_int_distribution<> chest(1, 100);
                int chestRoll = chest(gen);

                if (chestRoll <= 40) {
                    int gold = 8;
                    m_party->AddGold(gold);
                    std::cout << "  You pry open a rusted chest. +" << gold << " Gold!\n";
                } else if (chestRoll <= 90) {
                    auto alive = m_party->GetAliveMembers();
                    if (!alive.empty()) {
                        std::uniform_int_distribution<> idx(0, (int)alive.size() - 1);
                        auto& member = alive[idx(gen)];
                        int dmg = 8;
                        member->TakeDamage(dmg);
                        std::cout << "  A trap springs from the chest!\n";
                        std::cout << "  " << member->GetName() << " takes " << dmg << " damage!\n";
                    }
                } else {
                    auto alive = m_party->GetAliveMembers();
                    if (!alive.empty()) {
                        std::uniform_int_distribution<> idx(0, (int)alive.size() - 1);
                        auto& member = alive[idx(gen)];
                        AsciiArt::PrintSkull();
                        std::cout << "  Dark runes are carved inside the lid...\n";
                        std::cout << "  " << member->GetName() << " has been cursed!\n";
                        member->ApplyCurse(3);
                    }
                }
            }
            break;
        case LocationId::Mountains:
            // Blizzard: damages all party members
            {
                std::cout << "  A sudden blizzard sweeps through the peaks!\n";
                int dmg = 5;
                for (auto& m : m_party->GetMembers()) {
                    if (!m->IsAlive()) continue;
                    m->TakeDamage(dmg);
                    std::cout << "  " << m->GetName() << " takes " << dmg << " cold damage!\n";
                }
            }
            break;
        default:
            break;
    }
    AsciiArt::PrintThinDivider();
}

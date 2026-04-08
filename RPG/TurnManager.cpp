#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <algorithm>
#include <string>

#include "TurnManager.h"
#include "PlayerParty.h"
#include "Encounter.h"
#include "Character.h"
#include "InputManager.h"
#include "AsciiArt.h"
#include "Inventory.h"
#include "SmokeBomb.h"

#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"

#include "Goblin.h"
#include "Orc.h"
#include "Skeleton.h"
#include "Troll.h"
#include "Beggar.h"

#include "Merchant.h"

TurnManager::TurnManager() {}
TurnManager::~TurnManager() {}

static std::shared_ptr<Character> ChooseRandomAlive(const std::vector<std::shared_ptr<Character>>& list)
{
	std::vector<std::shared_ptr<Character>> alive;
	for (auto& e : list)
		if (e->IsAlive())
			alive.push_back(e);

	if (alive.empty()) return nullptr;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, (int)alive.size() - 1);
	return alive[dis(gen)];
}

// Print the enemy lineup with per-type ASCII art + HP bars
static void PrintEnemyStatus(const Encounter& encounter)
{
	AsciiArt::PrintThinDivider();
	std::cout << "  ENEMIES\n";
	int i = 0;
	for (auto& e : encounter.GetEnemies()) {
		if (!e->IsAlive()) { ++i; continue; }
		std::cout << "[" << i++ << "] ";
		AsciiArt::PrintHpBar(e->GetName(), e->GetHp(), e->GetMaxHp());
	}
	AsciiArt::PrintThinDivider();
}

// Print a single ASCII figure matching enemy type
static void PrintEnemyArt(const std::shared_ptr<Character>& e)
{
	if      (std::dynamic_pointer_cast<Goblin>(e))   AsciiArt::PrintGoblin();
	else if (std::dynamic_pointer_cast<Orc>(e))      AsciiArt::PrintOrc();
	else if (std::dynamic_pointer_cast<Skeleton>(e)) AsciiArt::PrintSkeleton();
	else if (std::dynamic_pointer_cast<Troll>(e))    AsciiArt::PrintTroll();
}

bool TurnManager::ExecuteEncounter(PlayerParty& party, Encounter& encounter, InputManager& input)
{
	// ── Beggar encounter ─────────────────────────────────────
	if (encounter.IsBeggarEncounter())
	{
		AsciiArt::PrintBeggar();
		std::cout << "A beggar approaches you, asking for some gold.\n";

		static std::random_device rd;
		static std::mt19937 gen(rd());

		bool done = false;
		while (!done)
		{
			std::cout << "\nWhat do you do?\n";
			std::cout << " 1) Attack\n 2) Give (one Gold)\n 3) Ignore\n> ";
			std::string choice = input.GetLine();

			if (choice == "1" || choice == "attack" || choice == "Attack")
			{
				std::cout << "You attack the beggar!\n";
				std::uniform_int_distribution<> loot(1,100);
				if (loot(gen) <= 50) { std::cout << "You found a coin.\n"; party.AddGold(1); }
				else std::cout << "You found nothing.\n";

				std::uniform_int_distribution<> cons(1,100);
				int c = cons(gen);
				if (c <= 49) {
					std::cout << "The Beggar is dead.\n";
				} else if (c <= 99) {
					auto alive = party.GetMembers();
					std::vector<std::shared_ptr<Character>> a;
					for (auto& m : alive) if (m->IsAlive()) a.push_back(m);
					if (!a.empty()) {
						std::uniform_int_distribution<> idx(0,(int)a.size()-1);
						auto t = a[idx(gen)];
						std::cout << "The Beggar hits " << t->GetName() << " for 5 damage!\n";
						t->TakeDamage(5);
					}
					std::cout << "Shame on you!\n";
				} else {
					auto alive = party.GetMembers();
					std::vector<std::shared_ptr<Character>> a;
					for (auto& m : alive) if (m->IsAlive()) a.push_back(m);
					if (!a.empty()) {
						std::uniform_int_distribution<> idx(0,(int)a.size()-1);
						auto t = a[idx(gen)];
						AsciiArt::PrintSkull();
						std::cout << t->GetName() << " has been cursed!\n";
						t->ApplyCurse(3);
					}
				}
				done = true;
			}
			else if (choice == "2" || choice == "give" || choice == "Give")
			{
				if (party.GetGold() < 1) { std::cout << "Not enough gold!\n"; continue; }
				std::cout << "You give one gold. \"Thank you, kind stranger...\"\n";
				party.AddGold(-1);
				done = true;
			}
			else if (choice == "3" || choice == "ignore" || choice == "Ignore")
			{
				std::cout << "You ignore the Beggar and walk on...\n";
				done = true;
			}
			else { std::cout << "Unknown command.\n"; }
		}
		return true;
	}

	// ── Merchant encounter ────────────────────────────────────
	if (encounter.IsMerchantEncounter())
	{
		AsciiArt::PrintMerchant();
		std::cout << "\"Over here, stranger!\"\n";
		Merchant merchant;
		merchant.Interact(party, input);
		std::cout << "You continue your journey.\n";
		return true;
	}

	// ── Combat encounter ──────────────────────────────────────
	bool isBoss = encounter.IsBossEncounter();
	if (isBoss) {
		AsciiArt::PrintBossWarning();
	} else {
		AsciiArt::PrintCombat();
	}

	// Show enemy art at combat start
	for (auto& e : encounter.GetEnemies())
		PrintEnemyArt(e);

	using EntityPtr = std::shared_ptr<Character>;

	while (true)
	{
		std::vector<EntityPtr> order;

		for (auto& p : party.GetMembers())
			if (p->IsAlive()) order.push_back(p);
		for (auto& e : encounter.GetEnemies())
			if (e->IsAlive()) order.push_back(e);

		std::sort(order.begin(), order.end(), [](const EntityPtr& a, const EntityPtr& b) {
			return a->GetSpeed() > b->GetSpeed();
		});

		for (auto& actor : order)
		{
			if (!actor->IsAlive()) continue;
			if (party.IsDefeated() || encounter.IsAllEnemiesDefeated()) break;

			// Reset defend at start of each actor's turn (bug fix)
			actor->ResetDefend();

			bool isPlayer = false;
			for (auto& p : party.GetMembers())
				if (p == actor) { isPlayer = true; break; }

			if (isPlayer)
			{
				// Show battlefield before each player turn
				AsciiArt::PrintThinDivider();
				AsciiArt::PrintHpBar(actor->GetName(), actor->GetHp(), actor->GetMaxHp());
				PrintEnemyStatus(encounter);

				bool turnDone = false;
				while (!turnDone)
				{
					std::string cmd = input.RequestActionFor(actor->GetName());

					if (cmd == "attack")
					{
						std::vector<EntityPtr> ae;
						for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) ae.push_back(e);
						if (ae.empty()) continue;
						int idx = input.RequestTragetIndex((int)ae.size());
						auto target = ae[idx];
						std::cout << actor->GetName() << " attacks " << target->GetName()
						          << " for " << actor->GetAttack() << " damage!\n";
						target->TakeDamage(actor->GetAttack());
						turnDone = true;
					}
					else if (cmd == "defend")
					{
						actor->Defend();
						std::cout << actor->GetName() << " braces for impact! (damage halved next hit)\n";
						turnDone = true;
					}
					else if (cmd == "skill")
					{
						std::vector<EntityPtr> ae;
						for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) ae.push_back(e);
						if (ae.empty()) continue;
						int idx = input.RequestTragetIndex((int)ae.size());
						auto target = ae[idx];

						if (auto w = std::dynamic_pointer_cast<Warrior>(actor))       w->ShieldBash(*target);
						else if (auto m = std::dynamic_pointer_cast<Mage>(actor))     m->Fireball(*target);
						else if (auto r = std::dynamic_pointer_cast<Ranger>(actor))   r->PrecisionShot(*target);
						else std::cout << actor->GetName() << " has no skills!\n";
						turnDone = true;
					}
					else if (cmd == "item")
					{
						auto& inv = party.GetInventory();
						const auto& stacks = inv.GetItemStacks();
						if (stacks.empty()) { std::cout << "Inventory empty!\n"; continue; }

						std::cout << "Inventory:\n";
						for (size_t i = 0; i < stacks.size(); ++i)
							std::cout << "[" << i << "] " << stacks[i].GetName()
							          << " x" << stacks[i].GetQuantity() << "\n";

						int itemIdx = input.RequestTragetIndex((int)stacks.size());
						if (itemIdx < 0 || itemIdx >= (int)stacks.size()) itemIdx = 0;
						auto choseName = stacks[itemIdx].GetName();

						// Smoke Bomb: escape without choosing a member target
						if (choseName == "Smoke Bomb") {
							inv.UseItem(choseName, *party.GetMembers()[0]); // target unused
							return true; // guaranteed escape
						}

						// All other items: choose a party member
						auto members = party.GetMembers();
						for (size_t i = 0; i < members.size(); ++i)
							std::cout << "[" << i << "] " << members[i]->GetName()
							          << " (HP: " << members[i]->GetHp() << "/" << members[i]->GetMaxHp() << ")\n";

						int tIdx = input.RequestTragetIndex((int)members.size());
						if (tIdx < 0 || tIdx >= (int)members.size()) tIdx = 0;
						inv.UseItem(choseName, *members[tIdx]);
						turnDone = true;
					}
					else if (cmd == "run")
					{
						if (isBoss) {
							std::cout << "You can't run from a Boss fight!\n";
							continue;
						}
						static std::random_device rd2;
						static std::mt19937 gen2(rd2());
						std::uniform_int_distribution<> dis(0, 1);
						if (dis(gen2) == 1) {
							std::cout << "You successfully fled!\n";
							return true;
						} else {
							std::cout << "Failed to escape! You take 5 damage!\n";
							actor->TakeDamage(5);
						}
						turnDone = true;
					}
					else if (cmd == "status")
					{
						party.PrintStatus();
					}
					else
					{
						std::cout << "Invalid command. (attack / defend / skill / item / run / status)\n";
					}
				}
			}
			else
			{
				// ── Enemy turn ────────────────────────────────────
				auto members = party.GetMembers();
				std::vector<EntityPtr> alivePlayers;
				for (auto& p : members) if (p->IsAlive()) alivePlayers.push_back(p);

				auto target = ChooseRandomAlive(alivePlayers);
				if (!target) continue;

				static std::random_device rd;
				static std::mt19937 gen(rd());
				std::uniform_int_distribution<> roll(0, 100);
				int r = roll(gen);

				if (auto t = std::dynamic_pointer_cast<Troll>(actor)) {
					// 20% BoulderSmash, 20% Regenerate, 60% normal attack
					if      (r <= 20) t->BoulderSmash(*target);
					else if (r <= 40) t->Regenerate();
					else {
						std::cout << actor->GetName() << " swings at " << target->GetName() << "!\n";
						target->TakeDamage(actor->GetAttack());
					}
				}
				else if (auto g = std::dynamic_pointer_cast<Goblin>(actor)) {
					if (r <= 30) g->ClubSmash(*target);
					else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
				}
				else if (auto o = std::dynamic_pointer_cast<Orc>(actor)) {
					if (r <= 30) o->AxeSwing(*target);
					else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
				}
				else if (auto s = std::dynamic_pointer_cast<Skeleton>(actor)) {
					if (r <= 30) s->BoneSlash(*target);
					else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
				}
				else {
					std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n";
					target->TakeDamage(actor->GetAttack());
				}
			}
		}

		if (party.IsDefeated()) {
			AsciiArt::PrintGameOver();
			return false;
		}

		if (encounter.IsAllEnemiesDefeated()) {
			AsciiArt::PrintVictory();

			int totalXp = 0, totalGold = 0;
			for (auto& e : encounter.GetEnemies()) {
				totalXp   += e->GetXpReward();
				totalGold += e->GetGoldReward();
			}
			std::cout << "Enemies defeated! +" << totalXp << " XP, +" << totalGold << " Gold!\n";
			AsciiArt::PrintThinDivider();

			party.DistributeXp(totalXp);
			party.AddGold(totalGold);

			for (auto& member : party.GetMembers()) {
				if (member->IsCursed()) {
					member->TickCurse();
					if (member->IsAlive())
						std::cout << member->GetName() << "'s curse tightens... "
						          << member->GetCurseRemainingFights() << " fight(s) left.\n";
				}
			}
			return true;
		}
	}

	return true;
}

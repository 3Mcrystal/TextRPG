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

#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"

#include "Goblin.h"
#include "Orc.h"
#include "Skeleton.h"
#include "Beggar.h"


TurnManager::TurnManager() {}
TurnManager::~TurnManager() {}

static std::shared_ptr<Character> ChooseRandomAlive(const std::vector<std::shared_ptr<Character>>& list)
{
	std::vector<std::shared_ptr<Character>> alive;
	for (auto& e : list)
		if (e->IsAlive())
			alive.push_back(e);

	if (alive.empty()) return nullptr;

	static std::random_device rd;//generate random seed
	static std::mt19937 gen(rd()); //mt19937, psudo-random number generator more viable than rand()
	std::uniform_int_distribution<> dis(0, alive.size() - 1); //uniform distribution between 0 and size-1

	return alive[dis(gen)]; //return random alive Character
}

bool TurnManager::ExecuteEncounter(PlayerParty& party, Encounter& encounter, InputManager& input)
{
	//Beggar encounter
	if (encounter.IsBeggarEncounter())
	{
		auto beggar = encounter.GetEnemies()[0]; //only one beggar
		std::cout << "A beggar approaches you, asking for some gold.\n";

		bool InteractionDone = false;
		while (!InteractionDone)
		{
			std::cout << "What do you do? : \n> ";
			std::cout << " 1) Attack\n";
			std::cout << " 2) Give (give one Gold)\n";
			std::cout << " 3) Ignore\n";

			std::string choice = input.GetLine();

			if (choice == "1" || choice == "Attack" || choice == "attack")
			{
				std::cout << "You attack the beggar!\n";

				//Loot(50%)
				static std::random_device rd;
				static std::mt19937 gen(rd());

				std::uniform_int_distribution<> lootRoll(1, 100);
				int loot = lootRoll(gen);
				if (loot <= 50)
				{
					std::cout << "You foud a coin the Beggar\n";
					party.AddGold(1);
				}
				else {

					std::cout << "You found nothing\n";
				}

				//Consequence (49%)
				std::uniform_int_distribution consRoll(1, 100);
				int cons = consRoll(gen);

				if (cons <= 49)
				{
					std::cout << "The Beggar is dead\n";
				}

				else if (cons <= 99) {
					auto members = party.GetMembers();
					std::vector<std::shared_ptr<Character>> Alivemembers;
					for (auto& m : members)
						if (m->IsAlive()) Alivemembers.push_back(m);

					if (!Alivemembers.empty()) {
						std::uniform_int_distribution<> idx(0, (int)Alivemembers.size() - 1);
						auto target = Alivemembers[idx(gen)];
						int dmg = 5;

						std::cout << "The Beggar hit you before passing away\n";
						std::cout << target->GetName() << " take " << dmg << " damage!\n";
						target->TakeDamage(dmg);

						std::cout << "Shame on your disgusting person!";

					}

				}
				else {

					auto members = party.GetMembers();
					std::vector<std::shared_ptr<Character>> AliveMembers;
					for (auto& m : members) {
						if (m->IsAlive()) AliveMembers.push_back(m);
					}

					if (!AliveMembers.empty()) {
						std::uniform_int_distribution<> idx(0, (int)AliveMembers.size() - 1);
						auto target = AliveMembers[idx(gen)];

						std::cout << "In his last breath you hear the Beggar laughing at you\n";
						std::cout << target->GetName() << "has been cursed\n";
						std::cout << "Well deserved\n";

						//TODO : curse Logic

					}

				}
				InteractionDone = true;
			}

			else if (choice == "2" || choice == "Give" || choice == "give")
			{

				std::cout << "You give one gold to the Beggar\n";
				std::cout << "Thank you\n";

				party.AddGold(-1);
				InteractionDone = true;
			}


			else if (choice == "3" || choice == "Ignore" || choice == "ignore")
			{
				std::cout << "You ignore the Beggar\n";
				std::cout << "You hear him crying as you continue your journey\n";
				InteractionDone = true;
			}

			else
			{

				std::cout << "Unknown command\n";

			}
		}

		return true;
	}


	using EntityPtr = std::shared_ptr<Character>;


	while (true)
	{

		std::vector<EntityPtr> order; //list entities in encounter

		//Party members
		for (auto& p : party.GetMembers())
			if (p->IsAlive())
				order.push_back(p);


		//Enemies
		for (auto& e : encounter.GetEnemies())
			if (e->IsAlive())
				order.push_back(e);


		//Sort by speed
		std::sort(order.begin(), order.end(), [](const EntityPtr& a, const EntityPtr& b) {
			return a->GetSpeed() > b->GetSpeed();

			//TODO : If buffers/debuffers

			}
		);

		//Turn resolution
		for (auto& actor : order)
		{
			if (!actor->IsAlive())
				continue; //skip dead entities

			//check end
			if (party.IsDefeated() || encounter.IsAllEnemiesDefeated()) break;

			bool IsPlayerActor = false;
			for (auto& p : party.GetMembers())
			{
				if (p == actor) {
					IsPlayerActor = true;
					break;
				}

			}

			if (IsPlayerActor)
			{
				bool TurnEnded = false;

				while (!TurnEnded)
				{

					std::string cmd = input.RequestActionFor(actor->GetName());

					//Attack
					if (cmd == "attack")
					{
						std::vector<EntityPtr> aliveEnemies;
						for (auto& e : encounter.GetEnemies())
							if (e->IsAlive())
								aliveEnemies.push_back(e);

						if (aliveEnemies.empty()) continue; //no targets

						int idx = input.RequestTragetIndex((int)aliveEnemies.size());
						auto target = aliveEnemies[idx];

						std::cout << actor->GetName() << " attacks " << target->GetName() << " for " << actor->GetAttack() << " damage!\n";
						target->TakeDamage(actor->GetAttack());
						TurnEnded = true;
					}

					//Defend
					else if (cmd == "defend")
					{
						actor->Defend();
						std::cout << actor->GetName() << " is defending!\n";
						TurnEnded = true;
					}

					//Skill
					else if (cmd == "skill")
					{
						std::vector<EntityPtr> aliveEnemies;
						for (auto& e : encounter.GetEnemies())
							if (e->IsAlive())
								aliveEnemies.push_back(e);

						if (aliveEnemies.empty()) continue; //no targets

						int idx = input.RequestTragetIndex((int)aliveEnemies.size());
						auto target = aliveEnemies[idx];

						//Warior Skill
						if (auto w = std::dynamic_pointer_cast<Warrior>(actor))
						{
							w->ShieldBash(*target);
						}

						//Mage Skill
						else if (auto m = std::dynamic_pointer_cast<Mage>(actor))
						{
							m->Fireball(*target);
						}

						//Ranger Skill
						else if (auto r = std::dynamic_pointer_cast<Ranger>(actor))
						{
							r->PrecisionShot(*target);
						}

						else
						{
							std::cout << actor->GetName() << " has no skills to use!\n";
						}
						TurnEnded = true;
					}

					else if (cmd == "item")
					{
						auto& inv = party.GetInventory();
						const auto& stacks = inv.GetItemStacks();

						if (stacks.empty())
						{
							std::cout << "Your inventory is empty!\n";
							continue; //continue turn
						}

						std::cout << "Inventory:\n";
						for (size_t i = 0; i < stacks.size(); ++i)
						{
							std::cout << "[" << i << "] " << stacks[i].GetName() << " x" << stacks[i].GetQuantity() << "\n";
						}

						int itemIdx = input.RequestTragetIndex((int)stacks.size());
						if (itemIdx < 0 || itemIdx >= (int)stacks.size()) itemIdx = 0;
						auto choseName = stacks[itemIdx].GetName();

						//Choose target to use item on
						auto members = party.GetMembers();

						for(size_t i = 0; i < members.size(); ++i)
						{
							std::cout << "[" << i << "] " << members[i]->GetName() << " (HP: " << members[i]->GetHp() << " / " << members[i]->GetMaxHp() << ")\n";
						}

						int targetIdx = input.RequestTragetIndex((int)members.size());
						if (targetIdx < 0 || targetIdx >= (int)members.size()) targetIdx = 0;
						auto target = members[targetIdx];

						//Use item
						inv.UseItem(choseName, *target);
						TurnEnded = true;
					}

					//Run (simple 50%)
					else if (cmd == "run")
					{
						static std::random_device rd;
						static std::mt19937 gen(rd());
						std::uniform_int_distribution<> dis(0, 1);
						if (dis(gen) == 1)
						{
							std::cout << "You successfully ran away!\n";
							return true; //escape
						}
						else //take damage for failed escape
						{
							std::cout << "Failed to run away! You take 5 damage!\n";
							actor->TakeDamage(5);
						}
						TurnEnded = true;
					}

					else if (cmd == "status")
					{
						party.PrintStatus();
					}

					else
					{
						std::cout << "Invalid command\n";
					}

				}
			}

			//Enemy turn (30% skill, 70% attack)
			else
			{
				auto enemies = party.GetMembers();
				std::vector<EntityPtr> alivePlayers;

				for (auto& p : enemies)
					if (p->IsAlive())
						alivePlayers.push_back(p);

				auto target = ChooseRandomAlive(alivePlayers);
				if (!target) continue; //no targets

				static std::random_device rd;
				static std::mt19937 gen(rd());
				std::uniform_int_distribution<> roll(0, 100);

				int r = roll(gen);

				//Goblin
				if (auto g = std::dynamic_pointer_cast<Goblin>(actor)) {

					if (r <= 30) g->ClubSmash(*target);

					else {
						int dmg = actor->GetAttack();
						std::cout << actor->GetName() << " attacks " << target->GetName() << "\n";
						target->TakeDamage(dmg);
					}
				}

				//Orc
				else if (auto o = std::dynamic_pointer_cast<Orc>(actor)) {
					if (r <= 30) o->AxeSwing(*target);
					else {
						int dmg = actor->GetAttack();
						std::cout << actor->GetName() << " attacks " << target->GetName() << "\n";
						target->TakeDamage(dmg);
					}
				}

				//Skeleton
				else if (auto s = std::dynamic_pointer_cast<Skeleton>(actor)) {
					if (r <= 30) s->BoneSlash(*target);
					else {
						int dmg = actor->GetAttack();
						std::cout << actor->GetName() << " attacks " << target->GetName() << "\n";
						target->TakeDamage(dmg);
					}
				}
				else
				{
					//Default attack
					int dmg = actor->GetAttack();
					std::cout << actor->GetName() << " attacks " << target->GetName() << "\n";
					target->TakeDamage(dmg);
				}

			} //end action turn


		}




		//Check Defeat
		if (party.IsDefeated())
		{
			std::cout << "Your party has been defeated...\n";
			std::cout << "Git good...\n";
			return false;
		}

		//Check Victory
		if (encounter.IsAllEnemiesDefeated())
		{
			std::cout << "You have defeated all enemies!\n";

			int totalXp = 0;
			int totalGold = 0;

			for (auto& e : encounter.GetEnemies())
			{
				totalXp += e->GetXpReward();
				totalGold += e->GetGoldReward();

			}

			std::cout << "Your party gains " << totalXp << " XP and " << totalGold << " Gold!\n";

			party.AddXp(totalXp);
			party.AddGold(totalGold);

			return true;
		}
	}

	return true;
}


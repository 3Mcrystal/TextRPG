#pragma once
#include <vector>
#include <memory>
#include "Inventory.h"

class Character;
class TurnManager;

class PlayerParty {
public:
	PlayerParty();
	~PlayerParty();

	void AddMember(std::shared_ptr<Character> ch);

	const std::vector<std::shared_ptr<Character>>& GetMembers() const;
	std::vector<std::shared_ptr<Character>>        GetAliveMembers() const;

	bool IsDefeated() const;
	void PrintStatus() const;

	int GetXp()   const;
	int GetGold() const;

	void AddXp(int amount);
	void DistributeXp(int amount);
	void AddGold(int amount);

	Inventory& GetInventory()             { return m_inventory; }
	const Inventory& GetInventory() const { return m_inventory; }

private:
	friend class TurnManager;
	friend class Merchant;

	std::vector<std::shared_ptr<Character>> m_members;
	int m_xp   = 0;
	int m_gold = 0;
	Inventory m_inventory;
};

#pragma once

#include <vector>
#include <memory>

#include "Inventory.h"

class Character;
class TurnManager;

class PlayerParty
{
public:
	PlayerParty();
	~PlayerParty();

	void AddMember(std::shared_ptr<Character> ch);

	const std::vector<std::shared_ptr<Character>>& GetMembers() const;

	bool IsDefeated() const;
	void PrintStatus() const;

	int GetXp() const;
	int GetGold() const;

	Inventory& GetInventory() { return m_inventory; }
	const Inventory& GetInventory() const { return m_inventory; }

private:
	friend class TurnManager; //Auth private access
	friend class Merchant;

	void AddXp(int amount);
	void AddGold(int amount);

	std::vector<std::shared_ptr<Character>> m_members;
	int m_xp;
	int m_gold;

	Inventory m_inventory;
};


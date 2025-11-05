#pragma once

#include <vector>
#include <memory>

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

private:
	friend class TurnManager; //Auth private access

	void AddXp(int amount);
	void AddGold(int amount);

	std::vector<std::shared_ptr<Character>> m_members;
	int m_xp;
	int m_gold;
};


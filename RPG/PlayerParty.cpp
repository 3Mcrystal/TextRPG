#include <iostream>

#include "PlayerParty.h"
#include "Character.h"

PlayerParty::PlayerParty()
	: m_xp(0), m_gold(0) {
}

PlayerParty::PlayerParty() {}

void PlayerParty::AddMember(std::shared_ptr<Character> ch) {

	m_members.push_back(ch);
}

const std::vector<std::shared_ptr<Character>>& PlayerParty::GetMembers() const {

	return m_members;
}

bool PlayerParty::IsDefeated() const {
	for (auto& m : m_members)
		if (m->IsAlive()) return false;
	return true;
}

void PlayerParty::PrintStatus() const {
	std::cout << "Party Status\n";
	for (auto& m : m_members) {
		std::cout << m->GetName() << " : " << m->GetHp() << " / " << m->GetMaxHp() << "\n";
	}
	std::cout << "XP : " << m_xp << " | Gold : " << m_gold << "\n";
}

int PlayerParty::GetXp() const { return m_xp;}
int PlayerParty::GetGold() const {return m_gold;}

void PlayerParty::AddXp(int amount) { m_xp += amount; }
void PlayerParty::AddGold(int amount) {m_gold += amount;}
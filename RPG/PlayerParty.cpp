#include <iostream>
#include "PlayerParty.h"
#include "Character.h"
#include "AsciiArt.h"

PlayerParty::PlayerParty() : m_xp(0), m_gold(0) {}
PlayerParty::~PlayerParty() {}

void PlayerParty::AddMember(std::shared_ptr<Character> ch) { m_members.push_back(ch); }

const std::vector<std::shared_ptr<Character>>& PlayerParty::GetMembers() const { return m_members; }

std::vector<std::shared_ptr<Character>> PlayerParty::GetAliveMembers() const {
	std::vector<std::shared_ptr<Character>> alive;
	for (auto& m : m_members) if (m->IsAlive()) alive.push_back(m);
	return alive;
}

bool PlayerParty::IsDefeated() const {
	for (auto& m : m_members) if (m->IsAlive()) return false;
	return true;
}

void PlayerParty::PrintStatus() const {
	AsciiArt::PrintDivider();
	std::cout << "  PARTY STATUS\n";
	AsciiArt::PrintThinDivider();
	for (auto& m : m_members) {
		std::string tag = m->IsAlive() ? "" : " [DEAD]";
		if (m->IsCursed())
			tag += " [CURSED x" + std::to_string(m->GetCurseRemainingFights()) + "]";
		std::cout << m->GetName() << "  Lv." << m->GetLevel() << tag << "\n";
		m->PrintLevelInfo();
	}
	AsciiArt::PrintThinDivider();
	std::cout << "Gold: " << m_gold << "\n";
	AsciiArt::PrintDivider();
}

int PlayerParty::GetXp()   const { return m_xp;   }
int PlayerParty::GetGold() const { return m_gold;  }

void PlayerParty::AddXp(int amount)   { m_xp   += amount; }
void PlayerParty::AddGold(int amount) { m_gold  += amount; }

void PlayerParty::DistributeXp(int amount) {
	m_xp += amount;
	for (auto& m : m_members) {
		if (m->IsAlive()) {
			std::cout << m->GetName() << " gains " << amount << " XP!\n";
			m->GainXp(amount);
		}
	}
}

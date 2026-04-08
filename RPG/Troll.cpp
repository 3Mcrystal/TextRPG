#include <iostream>
#include <algorithm>
#include "Troll.h"

Troll::Troll(const std::string& name)
	: Character(name, 80, 15, 4) // HP, ATK, SPD — tanky and slow
{}

Troll::~Troll() {}

void Troll::Update() {}

void Troll::BoulderSmash(Character& target)
{
	int dmg = GetAttack() + 10;
	std::cout << m_name << " HEAVES a boulder at " << target.GetName()
	          << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

void Troll::Regenerate()
{
	int heal = 12;
	int before = m_hp;
	m_hp = std::min(m_hp + heal, m_maxHp);
	int actual = m_hp - before;
	std::cout << m_name << " regenerates, recovering " << actual << " HP!\n";
}

int Troll::GetXpReward()   const { return 60; }
int Troll::GetGoldReward() const { return 25; }

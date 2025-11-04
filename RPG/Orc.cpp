#include "Orc.h"

#include <iostream>

Orc::Orc(const std::string& name)
	: Character(name, 40, 12, 6) //HP, ATK, SPD
{}

Orc::~Orc() {}

void Orc::Update() {}

void Orc::AxeSwing(Character& target) {
	int dmg = GetAttack() + 5;
	std::cout << m_name << " uses Axe Swing on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

int Orc::GetXpReward() const {
	return 18;
}
int Orc::GetGoldReward() const {
	return 8;
}
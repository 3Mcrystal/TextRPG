#include "Warrior.h"

#include <iostream>

Warrior::Warrior(const std::string& name)
	: Character(name, 50, 10, 8) //HP, ATK, SPD
{
	//TODO in case of speed buff/debuff
}

Warrior::~Warrior() {}

void Warrior::Update() {}

void Warrior::ShieldBash(Character& target) {
	int dmg = GetAttack() + 4;
	std::cout << m_name << " uses Shield Bash on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

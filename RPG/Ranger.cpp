#include "Ranger.h"

#include <iostream>

Ranger::Ranger(const std::string& name)
	: Character(name, 40, 9, 10) //HP, ATK, SPD
{
	//TODO in case of speed buff/debuff
}

Ranger::~Ranger() {}

void Ranger::Update() {}

void Ranger::PrecisionShot(Character& target) {
	int dmg = GetAttack() + 5;
	std::cout << m_name << " uses Precision Shot on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

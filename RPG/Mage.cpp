#include "Mage.h"

#include <iostream>

Mage::Mage(const std::string& name)
	: Character(name, 35, 12, 7) //HP, ATK, SPD
{
	//TODO in case of speed buff/debuff
}

Mage::~Mage() {}

void Mage::Update() {}

void Mage::Fireball(Character& target) {
	int dmg = GetAttack() + 6;
	std::cout << m_name << " casts Fireball on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}
#include <iostream>

#include "Potion.h"
#include "Character.h"

Potion::Potion(int healAmount, int price)
	: Item("Potion", price), m_healAmount(healAmount) {
}

Potion::~Potion() {}

void Potion::Use(Character& target)
{
	int heal = m_healAmount;
	int before = target.GetHp();

	int newHp = before + heal;
	if (newHp > target.GetMaxHp()) newHp = target.GetMaxHp();

	target.SetHp(newHp);

	std::cout << target.GetName() << " healed for " << (newHp - before) << " HP\n";
}
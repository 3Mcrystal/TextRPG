#include "Goblin.h"

#include <iostream>

Goblin::Goblin(const std::string& name)
	: Character(name, 20, 6, 9) //HP, ATK, SPD
{}

Goblin::~Goblin() {}

void Goblin::Update() {
	//simple AI, TurnManager decides
	//May add more complex behavior later
	//Probably for a last content update
}

void Goblin::ClubSmash(Character& target) {
	int dmg = GetAttack() + 3;
	std::cout << m_name << " uses Club Smash on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

int Goblin::GetXpReward() const {
	return 8;
}
int Goblin::GetGoldReward() const {
	return 3;
}
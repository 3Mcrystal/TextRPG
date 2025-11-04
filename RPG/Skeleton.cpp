#include "Skeleton.h"

#include <iostream>

Skeleton::Skeleton(const std::string& name)
	: Character(name, 25, 7, 11) //HP, ATK, SPD
{}

Skeleton::~Skeleton() {}

void Skeleton::Update() {}

void Skeleton::BoneSlash(Character& target) {
	int dmg = GetAttack() + 4;
	std::cout << m_name << " uses Bone Slash on " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

int Skeleton::GetXpReward() const {
	return 10;
}
int Skeleton::GetGoldReward() const {
	return 4;
}
#include "Beggar.h"

Beggar::Beggar(const std::string& name)
	: Character(name, 1, 0, 1) //HP, ATK, SPD
{}

Beggar::~Beggar() {}

void Beggar::Update() {}

int Beggar::GetGoldReward() const {
	return 2;
}

#pragma once
#include "Character.h"

class Mage : public Character {
public:
	Mage(const std::string& name = "Mage");
	virtual ~Mage();
	virtual void Update() override;
	void Fireball(Character& target);
	void ArcaneBlast(Character& target);
protected:
	virtual void OnLevelUp() override;
};

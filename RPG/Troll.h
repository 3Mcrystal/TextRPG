#pragma once
#include "Character.h"

class Troll : public Character
{
public:
	Troll(const std::string& name = "Troll");
	virtual ~Troll();

	virtual void Update() override;

	// Skills
	void BoulderSmash(Character& target);   // heavy single hit
	void Regenerate();                       // heals self

	virtual int GetXpReward()   const override;
	virtual int GetGoldReward() const override;
};

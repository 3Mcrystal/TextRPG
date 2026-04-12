#pragma once
#include "Character.h"

class Troll : public Character
{
public:
	Troll(const std::string& name = "Troll");
	virtual ~Troll();

	virtual void Update() override;

	// Skills
	void BoulderSmash(Character& target);
	void Regenerate();

	virtual int GetXpReward()   const override;
	virtual int GetGoldReward() const override;
};

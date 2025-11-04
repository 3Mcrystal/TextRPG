#pragma once

#include "Character.h"

class Goblin : public Character
{
public:
	Goblin(const std::string& name = "Goblin");
	virtual ~Goblin();

	virtual void Update() override;

	void ClubSmash(Character& target);

	virtual int GetXpReward() const override;
	virtual int GetGoldReward() const override;
};


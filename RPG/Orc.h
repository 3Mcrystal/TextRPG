#pragma once

#include "Character.h"

class Orc : public Character
{
	public:
	Orc(const std::string& name = "Orc");
	virtual ~Orc();

	virtual void Update() override;

	void AxeSwing(Character& target);
	
	virtual int GetXpReward() const override;
	virtual int GetGoldReward() const override;
};


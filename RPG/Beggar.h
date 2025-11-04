#pragma once

#include "Character.h"

class Beggar : public Character
{
	public:
	Beggar(const std::string& name = "Beggar");
	virtual ~Beggar();

	virtual void Update() override;

	virtual int GetGoldReward() const override;	
};


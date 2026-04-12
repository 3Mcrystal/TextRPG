#pragma once
#include "Character.h"

class Dragon : public Character {
public:
	Dragon(const std::string& name = "Ignarax the Dragon");
	virtual ~Dragon();
	virtual void Update() override;

	void DragonBreath(std::vector<std::shared_ptr<Character>>& targets);
	void TailSwipe(Character& target);

	virtual int GetXpReward()   const override;
	virtual int GetGoldReward() const override;
};

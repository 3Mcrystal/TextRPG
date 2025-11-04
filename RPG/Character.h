#pragma once

#include "Entity.h"

class Character : public Entity {
public:
	Character(const std::string& name, int maxHp, int attack, int speed);
	virtual ~Character();

	virtual void Update() override;
	int GetAttack() const;


	virtual int GetSpeed() const override;

	virtual void Defend();
	bool IsDefending() const;


	virtual int GetXpReward() const;
	virtual int GetGoldReward() const;

protected:

	int m_attack;
	int m_speed;
	bool m_defending;
	//int m_speedModifier, if I have time
};

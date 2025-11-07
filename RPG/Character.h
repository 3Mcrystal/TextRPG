#pragma once

#include "Entity.h"

class Character : public Entity {
public:
	Character(const std::string& name, int maxHp, int attack, int speed);
	virtual ~Character();

	virtual void Update() override;

	int GetAttack() const;

	virtual void TakeDamage(int amount) override;

	virtual int GetSpeed() const override;

	virtual void Defend();
	bool IsDefending() const;


	virtual int GetXpReward() const;
	virtual int GetGoldReward() const;

	void ApplyCurse(int fights);
	void TickCurse(); // Decrease curse duration by 1 after fight

	bool IsCursed() const { return m_isCursed; }
	int GetCurseRemainingFights() const { return m_curseRemainingFights; }

protected:

	int m_attack;
	int m_speed;
	bool m_defending;
	
	bool m_isCursed = false;
	int m_curseRemainingFights = 0;

	//int m_speedModifier, if I have time
};

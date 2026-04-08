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
	void ResetDefend(); // Call at start of each turn

	virtual int GetXpReward() const;
	virtual int GetGoldReward() const;

	void ApplyCurse(int fights);
	void RemoveCurse();             // Cleanly lifts the curse (used by Antidote)
	void TickCurse();               // Decrease curse duration by 1 after fight

	bool IsCursed() const { return m_isCursed; }
	int GetCurseRemainingFights() const { return m_curseRemainingFights; }

	// ── Leveling ─────────────────────────────────────────────
	int  GetLevel()     const { return m_level; }
	int  GetXp()        const { return m_xp; }
	int  GetXpToNext()  const;          // XP needed for next level
	void GainXp(int amount);            // adds XP, triggers LevelUp if ready
	void PrintLevelInfo() const;

protected:
	virtual void OnLevelUp();           // override in subclasses to boost class-specific stats

	int m_attack;
	int m_speed;
	bool m_defending;
	
	bool m_isCursed = false;
	int m_curseRemainingFights = 0;

	// ── Leveling ─────────────────────────────────────────────
	int m_level   = 1;
	int m_xp      = 0;

	// Base stats saved so level-up scales from originals
	int m_baseMaxHp;
	int m_baseAttack;
	int m_baseSpeed;
};

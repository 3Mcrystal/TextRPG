#pragma once
#include <string>
#include "Entity.h"
#include "StatusEffect.h"

class Character : public Entity {
public:
    Character(const std::string& name, int maxHp, int attack, int speed,
              int maxMp = 0, int defense = 0);
    virtual ~Character();

    virtual void Update() override;

    int GetAttack()  const;
    int GetDefense() const;

    virtual void TakeDamage(int amount) override;
    virtual int  GetSpeed() const override;

    virtual void Defend();
    bool IsDefending()  const;
    void ResetDefend();

    //MP
    int  GetMp()    const { return m_mp;    }
    int  GetMaxMp() const { return m_maxMp; }
    bool SpendMp(int cost);
    void RestoreMp(int amount);

    //Crit
    int  GetCritChance() const { return m_critChance; }
    bool RollCrit()      const;

    //Status effects
    void ApplyStatus(StatusType type, int duration, int power = 0);
    void TickStatus();          // called at start of actor's turn
    bool IsStunned()    const { return m_status.type == StatusType::Stun  && m_status.duration > 0; }
    bool IsPoisoned()   const { return m_status.type == StatusType::Poison && m_status.duration > 0; }
    bool IsBurning()    const { return m_status.type == StatusType::Burn   && m_status.duration > 0; }
    const StatusEffect& GetStatus() const { return m_status; }
    void ClearStatus()                    { m_status = {}; }

    //Rewards
    virtual int GetXpReward()   const;
    virtual int GetGoldReward() const;

    //Curse
    void ApplyCurse(int fights);
    void RemoveCurse();
    void TickCurse();
    bool IsCursed()                const { return m_isCursed; }
    int  GetCurseRemainingFights() const { return m_curseRemainingFights; }

    //Leveling 
    int  GetLevel()    const { return m_level; }
    int  GetXp()       const { return m_xp;    }
    int  GetXpToNext() const;
    void GainXp(int amount);
    void PrintLevelInfo() const;

    //Post-fight
    virtual void OnFightEnd();

protected:
    virtual void OnLevelUp();

    int  m_attack;
    int  m_speed;
    int  m_defense    = 0;
    bool m_defending  = false;

    int  m_mp         = 0;
    int  m_maxMp      = 0;
    int  m_critChance = 0;

    StatusEffect m_status;

    bool m_isCursed             = false;
    int  m_curseRemainingFights = 0;

    int m_level = 1;
    int m_xp    = 0;

    int m_baseMaxHp;
    int m_baseAttack;
    int m_baseSpeed;
    int m_baseMaxMp;
    int m_baseDefense;
};

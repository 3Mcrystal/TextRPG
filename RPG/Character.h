#pragma once

#include "Entity.h"

class Character : public Entity {
public:
    Character(const std::string& name, int maxHp, int attack);
    virtual ~Character();

    virtual void update() override;
    virtual int getAttack() const;
protected:
    int m_attack;
};

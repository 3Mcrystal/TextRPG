#pragma once
#include "Character.h"

class Warrior : public Character {
public:
    Warrior(const std::string& name = "Warrior");
    virtual ~Warrior();
    virtual void Update() override;

    void ShieldBash(Character& target);
    void Rally();
protected:
    virtual void OnLevelUp() override;
};

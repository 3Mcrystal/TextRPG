#pragma once
#include "Character.h"

class Ranger : public Character {
public:
    Ranger(const std::string& name = "Ranger");
    virtual ~Ranger();
    virtual void Update() override;

    void PrecisionShot(Character& target);
    void PoisonArrow(Character& target);
protected:
    virtual void OnLevelUp() override;
};

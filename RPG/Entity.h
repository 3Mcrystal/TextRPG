#pragma once

#include <string>

class Entity {
public:
    Entity(const std::string& name, int maxHp);
    virtual ~Entity();

	virtual void Update() = 0; // action per turn
    virtual void TakeDamage(int amount);
    virtual bool IsAlive() const;
    virtual std::string GetName() const;
    virtual int GetSpeed() const = 0;

    int GetHp() const;
    int GetMaxHp() const;

protected:
    std::string m_name;
    int m_hp;
    int m_maxHp;
};

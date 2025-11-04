#pragma once

#include <string>

class Entity {
public:
    Entity(const std::string& name, int maxHp);
    virtual ~Entity();

	virtual void update() = 0; // action per turn
    virtual void takeDamage(int amount);
    virtual bool isAlive() const;
    virtual std::string getName() const;

    int getHp() const;
    int getMaxHp() const;
protected:
    std::string m_name;
    int m_hp;
    int m_maxHp;
};

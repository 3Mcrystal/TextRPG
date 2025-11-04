#include "Entity.h"

Entity::Entity(const std::string& name, int maxHp)
    : m_name(name), m_hp(maxHp), m_maxHp(maxHp) {
}

Entity::~Entity() {}

void Entity::takeDamage(int amount) {
    m_hp -= amount;
    if (m_hp < 0) m_hp = 0;
}

bool Entity::isAlive() const { return m_hp > 0; }

std::string Entity::getName() const { return m_name; }

int Entity::getHp() const { return m_hp; }
int Entity::getMaxHp() const { return m_maxHp; }

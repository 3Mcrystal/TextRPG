#include "Entity.h"

Entity::Entity(const std::string& name, int maxHp)
    : m_name(name), m_hp(maxHp), m_maxHp(maxHp) {
}

Entity::~Entity() {}

void Entity::TakeDamage(int amount) {
    m_hp -= amount;
    if (m_hp < 0) m_hp = 0;
}

bool Entity::IsAlive() const { return m_hp > 0; }

std::string Entity::GetName() const { return m_name; }

void Entity::SetHp(int hp) {
    if (hp < 0) hp = 0;
    if (hp > m_maxHp) hp = m_maxHp;
    m_hp = hp;
}

void Entity::SetMaxHp(int maxHp) {
    if (maxHp < 1) maxHp = 1;
    m_maxHp = maxHp;
    if (m_hp > m_maxHp) m_hp = m_maxHp;
}

int Entity::GetHp() const { return m_hp; }
int Entity::GetMaxHp() const { return m_maxHp; }

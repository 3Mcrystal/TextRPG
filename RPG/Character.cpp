#include "Character.h"
#include <iostream>

Character::Character(const std::string& name, int maxHp, int attack)
    : Entity(name, maxHp), m_attack(attack) {
}

Character::~Character() {}

void Character::update() {
    //TODO
}

int Character::getAttack() const { return m_attack; }

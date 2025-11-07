#include <iostream>

#include "Character.h"

Character::Character(const std::string& name, int maxHp, int attack, int speed)
	: Entity(name, maxHp), m_attack(attack), m_speed(speed), m_defending(false) {
}

Character::~Character() {}

void Character::Update() {
	// TODO
}

int Character::GetAttack() const { return m_attack; }

int Character::GetSpeed() const { return m_speed; }

void Character::Defend() { m_defending = true; }

bool Character::IsDefending() const { return m_defending; }

int Character::GetGoldReward() const { return 2; } //default

int Character::GetXpReward() const { return 5; } //default

void Character::TakeDamage(int amount) {
	if (m_defending) {
		amount = amount / 2;
		m_defending = false;
	}
	Entity::TakeDamage(amount);
}

void Character::ApplyCurse(int fights) {
	m_isCursed = true;
	m_curseRemainingFights = fights;
}

void Character::TickCurse() {

	if (!m_isCursed) return;

	m_curseRemainingFights--;

	if (m_curseRemainingFights <= 0) {

		m_hp = 0; //die
		m_isCursed = false;
		std::cout << m_name << " has succumbed to the curse...\n";
	}

}
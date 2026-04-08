#include <iostream>
#include "Character.h"
#include "AsciiArt.h"

Character::Character(const std::string& name, int maxHp, int attack, int speed)
	: Entity(name, maxHp)
	, m_attack(attack), m_speed(speed), m_defending(false)
	, m_baseMaxHp(maxHp), m_baseAttack(attack), m_baseSpeed(speed)
{}

Character::~Character() {}

void Character::Update() {}

int Character::GetAttack() const { return m_attack; }
int Character::GetSpeed()  const { return m_speed;  }

void Character::Defend()         { m_defending = true;  }
bool Character::IsDefending()    const { return m_defending; }
void Character::ResetDefend()    { m_defending = false; }

int Character::GetGoldReward() const { return 2; }
int Character::GetXpReward()   const { return 5; }

void Character::TakeDamage(int amount) {
	if (m_defending) {
		amount = amount / 2;
		// defending resets at start of next turn via ResetDefend(), not here
	}
	Entity::TakeDamage(amount);
}

void Character::ApplyCurse(int fights) {
	m_isCursed = true;
	m_curseRemainingFights = fights;
}

void Character::RemoveCurse() {
	m_isCursed = false;
	m_curseRemainingFights = 0;
}

void Character::TickCurse() {
	if (!m_isCursed) return;
	m_curseRemainingFights--;
	if (m_curseRemainingFights <= 0) {
		m_hp = 0;
		m_isCursed = false;
		std::cout << m_name << " has succumbed to the curse...\n";
	}
}

// ── Leveling ─────────────────────────────────────────────────

// XP required to reach the NEXT level (scales with current level)
int Character::GetXpToNext() const {
	return 20 + (m_level - 1) * 15; // lvl1->2: 20, lvl2->3: 35, etc.
}

void Character::GainXp(int amount) {
	m_xp += amount;
	while (m_xp >= GetXpToNext()) {
		m_xp -= GetXpToNext();
		m_level++;
		OnLevelUp();
	}
}

void Character::OnLevelUp() {
	// Base stat growth every level
	m_baseMaxHp  += 8;
	m_baseAttack += 2;
	m_maxHp       = m_baseMaxHp;
	m_attack      = m_baseAttack;

	// Partial heal on level-up (feel good moment)
	m_hp = std::min(m_hp + 15, m_maxHp);

	AsciiArt::PrintLevelUp();
	std::cout << m_name << " reached level " << m_level << "!\n";
	std::cout << "  MaxHP  +" << 8  << "  (now " << m_maxHp  << ")\n";
	std::cout << "  Attack +" << 2  << "  (now " << m_attack  << ")\n";
	std::cout << "  Restored 15 HP.\n";
	AsciiArt::PrintThinDivider();
}

void Character::PrintLevelInfo() const {
	AsciiArt::PrintXpBar(m_xp, GetXpToNext(), m_level);
}
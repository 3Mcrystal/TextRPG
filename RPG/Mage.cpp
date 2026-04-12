#include <iostream>
#include "Mage.h"
#include "AsciiArt.h"

//HP, ATK, SPD, MP, DEF
Mage::Mage(const std::string& name)
	: Character(name, 35, 14, 7, 50, 0) {}

Mage::~Mage() {}
void Mage::Update() {}

void Mage::Fireball(Character& target) {
	const int COST = 10;
	if (!SpendMp(COST)) {
		std::cout << m_name << " doesn't have enough MP for Fireball! (" << COST << " needed)\n";
		return;
	}
	int dmg = GetAttack() + 6;
	std::cout << m_name << " hurls a FIREBALL at " << target.GetName() << " for " << dmg << " damage!\n";
	target.TakeDamage(dmg);
}

void Mage::ArcaneBlast(Character& target) {
	const int COST = 18;
	if (!SpendMp(COST)) {
		std::cout << m_name << " doesn't have enough MP for Arcane Blast! (" << COST << " needed)\n";
		return;
	}
	int dmg = GetAttack() + 16;
	std::cout << m_name << " unleashes ARCANE BLAST on " << target.GetName() << " for " << dmg << " damage!!\n";
	target.TakeDamage(dmg);
}

void Mage::OnLevelUp() {
	m_baseMaxHp  += 5;
	m_baseAttack += 4;
	m_baseMaxMp  += 12;
	m_baseSpeed  += 0;

	m_maxHp  = m_baseMaxHp;
	m_attack = m_baseAttack;
	m_maxMp  = m_baseMaxMp;
	m_hp     = std::min(m_hp + 10, m_maxHp);
	m_mp     = std::min(m_mp + 15, m_maxMp);

	AsciiArt::PrintLevelUp();
	std::cout << m_name << " (Mage) reached level " << m_level << "!\n";
	std::cout << "  MaxHP  + 5 (now " << m_maxHp  << ")\n";
	std::cout << "  Attack + 4 (now " << m_attack  << ")\n";
	std::cout << "  MaxMP  +12 (now " << m_maxMp   << ")\n";
	AsciiArt::PrintThinDivider();
}

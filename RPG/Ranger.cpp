#include <iostream>
#include "Ranger.h"
#include "AsciiArt.h"

Ranger::Ranger(const std::string& name)
    : Character(name, 40, 10, 12, 40, 1)
{
    m_critChance = 15;
}

Ranger::~Ranger() {}
void Ranger::Update() {}

void Ranger::PrecisionShot(Character& target) {
    const int COST = 12;
    if (!SpendMp(COST)) {
        std::cout << m_name << " needs " << COST << " MP for Precision Shot!\n"; return;
    }
	int dmg = GetAttack() * 2 + 4; //Crit guranteed
    std::cout << m_name << " takes careful aim... PRECISION SHOT on "
              << target.GetName() << " for " << dmg << " damage!\n";
    target.TakeDamage(dmg);
}

void Ranger::PoisonArrow(Character& target) {
    const int COST = 10;
    if (!SpendMp(COST)) {
        std::cout << m_name << " needs " << COST << " MP for Poison Arrow!\n"; return;
    }
    int dmg = GetAttack();
    std::cout << m_name << " fires a POISON ARROW at " << target.GetName()
              << " for " << dmg << " damage!\n";
    target.TakeDamage(dmg);
    target.ApplyStatus(StatusType::Poison, 3, 4); //3 turns, 4 dmg/turn
}

void Ranger::OnLevelUp() {
    m_baseMaxHp  += 7;
    m_baseAttack += 2;
    m_baseSpeed  += 1;
    m_baseMaxMp  += 6;
    m_critChance  = std::min(m_critChance + 3, 50);

    m_maxHp  = m_baseMaxHp;
    m_attack = m_baseAttack;
    m_speed  = m_baseSpeed;
    m_maxMp  = m_baseMaxMp;
    m_hp = std::min(m_hp + 12, m_maxHp);
    m_mp = std::min(m_mp + 8,  m_maxMp);

    AsciiArt::PrintLevelUp();
    std::cout << m_name << " (Ranger) reached level " << m_level << "!\n";
    std::cout << "  MaxHP  + 7 (now " << m_maxHp     << ")\n";
    std::cout << "  Attack + 2 (now " << m_attack     << ")\n";
    std::cout << "  Speed  + 1 (now " << m_speed      << ")\n";
    std::cout << "  Crit   + 3 (now " << m_critChance << "%)\n";
    AsciiArt::PrintThinDivider();
}

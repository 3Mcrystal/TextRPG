#include <iostream>
#include <random>
#include <algorithm>
#include "Warrior.h"
#include "AsciiArt.h"

Warrior::Warrior(const std::string& name)
    : Character(name, 55, 10, 8, 30, 3) {}

Warrior::~Warrior() {}
void Warrior::Update() {}

void Warrior::ShieldBash(Character& target) {
    const int COST = 8;
    if (!SpendMp(COST)) {
        std::cout << m_name << " needs " << COST << " MP for Shield Bash!\n"; return;
    }
    int dmg = GetAttack() + 5;
    std::cout << m_name << " SHIELD BASHES " << target.GetName()
              << " for " << dmg << " damage!\n";
    target.TakeDamage(dmg);

    //Can stun 50%
    static std::random_device rd; static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1,100);
    if (dis(gen) <= 50)
        target.ApplyStatus(StatusType::Stun, 1);
}
//Self heal
void Warrior::Rally() {
    const int COST = 6;
    if (!SpendMp(COST)) {
        std::cout << m_name << " needs " << COST << " MP for Rally!\n"; return;
    }
    int heal = 15 + (m_level * 2);
    int before = m_hp;
    m_hp = std::min(m_hp + heal, m_maxHp);
    int gained = m_hp - before;
    std::cout << m_name << " rallies, recovering " << gained << " HP!\n";
}

void Warrior::OnLevelUp() {
    m_baseMaxHp   += 12;
    m_baseAttack  += 2;
    m_baseDefense += 1;
    m_baseMaxMp   += 3;

    m_maxHp   = m_baseMaxHp;
    m_attack  = m_baseAttack;
    m_defense = m_baseDefense;
    m_maxMp   = m_baseMaxMp;
    m_hp  = std::min(m_hp + 20, m_maxHp);
    m_mp  = std::min(m_mp + 5,  m_maxMp);

    AsciiArt::PrintLevelUp();
    std::cout << m_name << " (Warrior) reached level " << m_level << "!\n";
    std::cout << "  MaxHP  +12 (now " << m_maxHp   << ")\n";
    std::cout << "  Attack + 2 (now " << m_attack   << ")\n";
    std::cout << "  Defense+ 1 (now " << m_defense  << ")\n";
    std::cout << "  MaxMP  + 3 (now " << m_maxMp    << ")\n";
    AsciiArt::PrintThinDivider();
}

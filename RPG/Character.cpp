#include <iostream>
#include <random>
#include <algorithm>
#include "Character.h"
#include "AsciiArt.h"

Character::Character(const std::string& name, int maxHp, int attack, int speed,
    int maxMp, int defense)
    : Entity(name, maxHp)
    , m_attack(attack), m_speed(speed), m_defense(defense)
    , m_mp(maxMp), m_maxMp(maxMp)
    , m_baseMaxHp(maxHp), m_baseAttack(attack), m_baseSpeed(speed)
    , m_baseMaxMp(maxMp), m_baseDefense(defense)
{}

Character::~Character() {}
void Character::Update() {}

int  Character::GetAttack()  const { return m_attack; }
void Character::SetAttack(int attack) { m_attack = attack; }
int  Character::GetDefense() const { return m_defense; }
int  Character::GetSpeed()   const { return m_speed; }

void Character::Defend() { m_defending = true; }
bool Character::IsDefending() const { return m_defending; }
void Character::ResetDefend() { m_defending = false; }

bool Character::SpendMp(int cost) {
    if (m_mp < cost) return false;
    m_mp -= cost;
    return true;
}

void Character::RestoreMp(int amount) {
    m_mp = std::min(m_mp + amount, m_maxMp);
}

bool Character::RollCrit() const {
    if (m_critChance <= 0) return false;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    return dis(gen) <= m_critChance;
}

//Status effects
void Character::ApplyStatus(StatusType type, int duration, int power) {
    //Don't stack stun
    if (m_status.IsActive() && m_status.type == StatusType::Stun) return;
    m_status.type = type;
    m_status.duration = duration;
    m_status.power = power;
    std::cout << m_name << " is now " << m_status.Name() << "ed!\n";
}

void Character::TickStatus() {
    if (!m_status.IsActive()) return;

    switch (m_status.type) {
    case StatusType::Poison:
    case StatusType::Burn:
        std::cout << m_name << " suffers " << m_status.power
            << " " << m_status.Name() << " damage!\n";
        Entity::TakeDamage(m_status.power); //Bypass defense
        break;
    case StatusType::Stun:
        std::cout << m_name << " is stunned and loses their turn!\n";
        break;
    default: break;
    }

    m_status.duration--;
    if (m_status.duration <= 0) {
        std::cout << m_name << "'s " << m_status.Name() << " fades.\n";
        m_status = {};
    }
}

//Rewards
int Character::GetGoldReward() const { return 2; }
int Character::GetXpReward()   const { return 5; }

//Damage
void Character::TakeDamage(int amount) {
    if (m_defending) {
        amount = std::max(0, amount - m_defense);
        amount = amount / 2;
    }
    Entity::TakeDamage(amount);
}

//Curse
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

//Post-fight
void Character::OnFightEnd() {
    int restore = std::max(1, m_maxMp * 30 / 100);
    RestoreMp(restore);
    //Status effects stay after fight
}

//Leveling
int Character::GetXpToNext() const {
    return 20 + (m_level - 1) * 15;
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
    m_baseMaxHp += 8;
    m_baseAttack += 2;
    m_maxHp = m_baseMaxHp;
    m_attack = m_baseAttack;
    m_hp = std::min(m_hp + 15, m_maxHp);

    AsciiArt::PrintLevelUp();
    std::cout << m_name << " reached level " << m_level << "!\n";
    std::cout << "  MaxHP  +" << 8 << " (now " << m_maxHp << ")\n";
    std::cout << "  Attack +" << 2 << " (now " << m_attack << ")\n";
    std::cout << "  Restored 15 HP.\n";
    AsciiArt::PrintThinDivider();
}

void Character::PrintLevelInfo() const {
    AsciiArt::PrintHpBar(m_name, m_hp, m_maxHp);
    if (m_maxMp > 0)
        AsciiArt::PrintMpBar(m_name, m_mp, m_maxMp);
    AsciiArt::PrintXpBar(m_xp, GetXpToNext(), m_level);
    if (m_status.IsActive())
        std::cout << "  [" << m_status.Name() << " " << m_status.duration << " turns]\n";
}
#include "Encounter.h"
#include "Character.h"

Encounter::Encounter() : m_isBeggar(false) {}
Encounter::~Encounter() {}

void Encounter::AddEnemy(std::shared_ptr<Character> enemy) {
    m_enemies.push_back(enemy);
}

const std::vector<std::shared_ptr<Character>>& Encounter::GetEnemies() const {
    return m_enemies;
}

bool Encounter::IsBeggarEncounter() const {
    return m_isBeggar;
}

void Encounter::SetBeggarEncounter(bool b) {
    m_isBeggar = b;
}

bool Encounter::IsAllEnemiesDefeated() const {
    for (auto& e : m_enemies) {
        if (e->IsAlive()) return false;
    }
    return true;
}

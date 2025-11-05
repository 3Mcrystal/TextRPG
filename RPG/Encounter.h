#pragma once

#include <vector>
#include <memory>

class Character;

class Encounter {
public:
    Encounter();
    ~Encounter();

    void AddEnemy(std::shared_ptr<Character> enemy);
    const std::vector<std::shared_ptr<Character>>& GetEnemies() const;

    bool IsBeggarEncounter() const;
    void SetBeggarEncounter(bool b);

    bool IsAllEnemiesDefeated() const;
private:
    std::vector<std::shared_ptr<Character>> m_enemies;
    bool m_isBeggar;
};

#pragma once

#include<vector>
#include<memory>

class Chracter;

class Encounter
{
public:
	Encounter();
	~Encounter();

	void AddEnemy(std::shared_ptr<Character> enemy);
	const std::vector<std::shared_ptr<Character>>& GetEnemy() const;

	bool IsBeggarEncounter() const;
	void SetBeggarEncounter(bool b);

	bool IsAllEnemiesDefeated() const;

private:
	std::vector<std::shared_ptr<Character>> m_enemy;
	bool m_isbeggar;
};


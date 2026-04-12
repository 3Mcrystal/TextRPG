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

	bool IsBeggarEncounter()  const;
	void SetBeggarEncounter(bool b);

	bool IsMerchantEncounter() const;
	void SetMerchantEncounter(bool b);

	bool IsBossEncounter() const;
	void SetBossEncounter(bool b);

	bool IsFinalBoss() const;
	void SetFinalBoss(bool b);

	bool IsAllEnemiesDefeated() const;

private:
	std::vector<std::shared_ptr<Character>> m_enemies;
	bool m_isBeggar   = false;
	bool m_isMerchant = false;
	bool m_isBoss     = false;
	bool m_isFinal    = false;
};

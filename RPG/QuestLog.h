#pragma once
#include <string>
#include <vector>

enum class QuestType {
	KillGoblins,
	KillOrcs,
	KillSkeletons,
	SurviveFights,
	EarnGold
};

struct Quest {
	QuestType type;
	std::string name;
	std::string description;
	int goal;
	int progress = 0;
	bool completed = false;

	// Rewards
	int xpReward;
	int goldReward;
};

class QuestLog {
public:
	QuestLog();
	void GenerateQuests();
	void PrintBoard() const;

	void OnKill(const std::string& enemyName);
	void OnFightSurvived();
	void OnGoldEarned(int amount);

	struct Reward { int xp; int gold; std::string questName; };
	std::vector<Reward> CollectCompletedRewards();

	bool HasActiveQuests() const;
	void RefreshCompleted();

private:
	std::vector<Quest> m_quests;

	Quest MakeQuest(QuestType type) const;
};

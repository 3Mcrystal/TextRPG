#include <iostream>
#include "QuestLog.h"
#include "AsciiArt.h"

QuestLog::QuestLog() {
	GenerateQuests();
}

Quest QuestLog::MakeQuest(QuestType type) const {
	Quest q;
	q.type = type;
	q.progress  = 0;
	q.completed = false;
	switch (type) {
		case QuestType::KillGoblins:
			q.name        = "Goblin Slayer";
			q.description = "Kill 3 Goblins";
			q.goal        = 3;
			q.xpReward    = 30;
			q.goldReward  = 10;
			break;
		case QuestType::KillOrcs:
			q.name        = "Orc Hunter";
			q.description = "Kill 2 Orcs";
			q.goal        = 2;
			q.xpReward    = 40;
			q.goldReward  = 15;
			break;
		case QuestType::KillSkeletons:
			q.name        = "Bone Collector";
			q.description = "Destroy 3 Skeletons";
			q.goal        = 3;
			q.xpReward    = 35;
			q.goldReward  = 12;
			break;
		case QuestType::SurviveFights:
			q.name        = "Hardened Warriors";
			q.description = "Survive 3 encounters";
			q.goal        = 3;
			q.xpReward    = 45;
			q.goldReward  = 20;
			break;
		case QuestType::EarnGold:
			q.name        = "Treasure Hunters";
			q.description = "Earn 30 Gold in battle";
			q.goal        = 30;
			q.xpReward    = 25;
			q.goldReward = 0; //the power of frienship
			break;
	}
	return q;
}

void QuestLog::GenerateQuests() {
	m_quests.clear();
	//Always 3 quests, rotate types
	m_quests.push_back(MakeQuest(QuestType::KillGoblins));
	m_quests.push_back(MakeQuest(QuestType::KillOrcs));
	m_quests.push_back(MakeQuest(QuestType::SurviveFights));
}

void QuestLog::PrintBoard() const {
	AsciiArt::PrintDivider();
	std::cout << "  QUEST BOARD\n";
	AsciiArt::PrintThinDivider();
	if (m_quests.empty()) {
		std::cout << "  No active quests. Explore to unlock more.\n";
	}
	for (size_t i = 0; i < m_quests.size(); ++i) {
		const auto& q = m_quests[i];
		std::string status = q.completed ? " [DONE]" : "";
		std::cout << "[" << i << "] " << q.name << status << "\n";
		std::cout << "    " << q.description
		          << " (" << q.progress << "/" << q.goal << ")\n";
		std::cout << "    Reward: " << q.xpReward << " XP";
		if (q.goldReward > 0) std::cout << " + " << q.goldReward << " Gold";
		std::cout << "\n";
	}
	AsciiArt::PrintDivider();
}

void QuestLog::OnKill(const std::string& enemyName) {
	for (auto& q : m_quests) {
		if (q.completed) continue;
		if (q.type == QuestType::KillGoblins   && enemyName == "Goblin")   q.progress++;
		if (q.type == QuestType::KillOrcs      && enemyName == "Orc")      q.progress++;
		if (q.type == QuestType::KillSkeletons && enemyName == "Skeleton") q.progress++;
		if (q.progress >= q.goal) q.completed = true;
	}
}

void QuestLog::OnFightSurvived() {
	for (auto& q : m_quests) {
		if (q.completed) continue;
		if (q.type == QuestType::SurviveFights) {
			q.progress++;
			if (q.progress >= q.goal) q.completed = true;
		}
	}
}

void QuestLog::OnGoldEarned(int amount) {
	for (auto& q : m_quests) {
		if (q.completed) continue;
		if (q.type == QuestType::EarnGold) {
			q.progress += amount;
			if (q.progress >= q.goal) q.completed = true;
		}
	}
}

std::vector<QuestLog::Reward> QuestLog::CollectCompletedRewards() {
	std::vector<Reward> out;
	for (auto& q : m_quests) {
		if (q.completed && q.xpReward > 0) {
			out.push_back({ q.xpReward, q.goldReward, q.name });
			q.xpReward   = 0; //mark as collected
			q.goldReward = 0;
		}
	}
	return out;
}

bool QuestLog::HasActiveQuests() const {
	for (auto& q : m_quests)
		if (!q.completed) return true;
	return false;
}

//Maybe be randomized later, hardcoded for now
void QuestLog::RefreshCompleted() {
    // Replace if all done
    bool anyActive = false;
    for (auto& q : m_quests)
        if (!q.completed || q.xpReward > 0) { anyActive = true; break; }

    if (!anyActive) {
        //Rotate,
        m_quests.clear();
        m_quests.push_back(MakeQuest(QuestType::KillSkeletons));
        m_quests.push_back(MakeQuest(QuestType::EarnGold));
        m_quests.push_back(MakeQuest(QuestType::SurviveFights));
    }
}

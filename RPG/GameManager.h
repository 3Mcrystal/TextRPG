#pragma once
#include <memory>
#include "WorldMap.h"
#include "QuestLog.h"

class InputManager;
class TurnManager;
class PlayerParty;
class Encounter;

class GameManager {
public:
	GameManager();
	~GameManager();

	void start();
	void loop();
	void shutdown();

private:
	std::unique_ptr<InputManager> m_input;
	std::unique_ptr<TurnManager>  m_turns;
	std::unique_ptr<PlayerParty>  m_party;
	std::unique_ptr<WorldMap>     m_world;
	std::unique_ptr<QuestLog>     m_quests;

	bool m_running      = false;
	bool m_gameWon      = false;
	int  m_fightCount   = 0;   // combat wins only
	bool m_canRest      = true;
	bool m_finalUnlocked = false; // set when fightCount hits threshold

	// ── encounter generation ──────────────────────────────────
	std::unique_ptr<Encounter> GenerateEncounter();
	std::unique_ptr<Encounter> GenerateCombatEncounter(int scalingLevel);
	std::unique_ptr<Encounter> GenerateBossEncounter(int scalingLevel);
	std::unique_ptr<Encounter> GenerateFinalBoss();

	// ── town actions ─────────────────────────────────────────
	void DoTownMenu();
	void DoRest(bool free);
	void DoInventory() const;
	void PrintHelp() const;
	void RollLocationEvent();

	// ── quest rewards ─────────────────────────────────────────
	void CollectQuestRewards();
};

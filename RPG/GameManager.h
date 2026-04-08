#pragma once

#include <memory>

class InputManager;
class TurnManager;
class PlayerParty;
class Encounter;

class GameManager
{
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

	bool m_running;
	int  m_fightCount  = 0;   // incremented after each combat win
	bool m_canRest     = true; // cooldown: only one rest per encounter

	std::unique_ptr<Encounter> GenerateRandomEncounter();
	std::unique_ptr<Encounter> GenerateBossEncounter();
	void DoRest();
};

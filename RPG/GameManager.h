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
	std::unique_ptr<TurnManager> m_turns;
	std::unique_ptr<PlayerParty> m_party;

	bool m_running;

	std::unique_ptr<Encounter> GenerateRandomEncounter();
};
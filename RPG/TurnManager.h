#pragma once

#include <memory>

class PlayerParty;
class Encounter;
class InputManager;

class TurnManager {
public:
	TurnManager();
	~TurnManager();

	bool ExecuteEncounter(PlayerParty& party, Encounter& encounter, InputManager& input);
};
#pragma once
#include <memory>

class PlayerParty;
class Encounter;
class InputManager;
class QuestLog;

enum class EncounterResult {
	Victory, //Want a medal?
	Fled, // Coward: run / smoke bomb
	Defeat //Party dead: loser
};

class TurnManager {
public:
    TurnManager();
    ~TurnManager();

    EncounterResult ExecuteEncounter(PlayerParty& party, Encounter& encounter,
                                     InputManager& input, QuestLog* quests = nullptr);
};

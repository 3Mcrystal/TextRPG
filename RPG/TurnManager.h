#pragma once

#include <vector>
#include <memory>

class Entity;

class TurnManager {
public:
	TurnManager();
	~TurnManager();

	void AddEntity(std::shared_ptr<Entity> entity);
	void StartTurnCycle();

private:
	std::vector<std::shared_ptr<Entity>> m_entities;
};
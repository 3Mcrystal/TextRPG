#pragma once

#include <vector>
#include <memory>

class Entity;

class TurnManager {
public:
	TurnManager();
	~TurnManager();

	void addEntity(std::shared_ptr<Entity> entity);
	void startTurnCycle();

private:
	std::vector<std::shared_ptr<Entity>> m_entities;
};
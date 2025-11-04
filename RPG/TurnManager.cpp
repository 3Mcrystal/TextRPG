#include "TurnManager.h"
#include "Entity.h"

TurnManager::TurnManager() {}
TurnManager::~TurnManager() {}

void TurnManager::AddEntity(std::shared_ptr<Entity> entity) {
	m_entities.push_back(entity);
}

void TurnManager::StartTurnCycle() {
	//On each alive entity, call update

	for (auto& entity : m_entities) {
		if (entity->isAlive()) {
			entity->update();
		}
	}
}

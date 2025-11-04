#pragma once

#include <memory>

class InputManager;
class TurnManager;

class GameManager
{
public:
	GameManager();
	~GameManager();

	void start();
	void loop();
	void shutdown();

private:
	bool m_running;

	std::unique_ptr<InputManager> m_input;
	std::unique_ptr<TurnManager> m_turns;
};


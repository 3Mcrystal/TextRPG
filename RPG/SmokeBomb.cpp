#include <iostream>
#include "SmokeBomb.h"
#include "Character.h"

SmokeBomb::SmokeBomb(int price)
	: Item("Smoke Bomb", price) {}

SmokeBomb::~SmokeBomb() {}

void SmokeBomb::Use(Character& target)
{
	// The target parameter is unused for escape items — the effect
	// is on the whole party. TurnManager checks WasUsedToEscape().
	(void)target;
	m_escapeTriggered = true;
	std::cout << "A thick cloud of smoke fills the air... you slip away!\n";
}

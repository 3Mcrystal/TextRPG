#pragma once
#include "Item.h"

// When used in combat, signals the TurnManager to flee.
// Since Use() only receives the target Character, we set a flag
// on the item itself that TurnManager checks after calling UseItem().
class SmokeBomb : public Item
{
public:
	SmokeBomb(int price = 8);
	virtual ~SmokeBomb();

	virtual void Use(Character& target) override;

	bool WasUsedToEscape() const { return m_escapeTriggered; }
	void ResetEscapeFlag() { m_escapeTriggered = false; }

private:
	bool m_escapeTriggered = false;
};

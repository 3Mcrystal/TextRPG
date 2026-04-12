#pragma once
#include "Item.h"

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

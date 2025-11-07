#pragma once

#include "Item.h"

class Potion : public Item
{
public:
	Potion(int healAmount = 20, int price = 5);
	virtual ~Potion();

	virtual void Use(Character& target) override;

private:
	int m_healAmount;
};

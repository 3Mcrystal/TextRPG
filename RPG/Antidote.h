#pragma once
#include "Item.h"

class Antidote : public Item
{
public:
	Antidote(int price = 12);
	virtual ~Antidote();

	virtual void Use(Character& target) override;
};

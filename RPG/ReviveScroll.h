#pragma once
#include "Item.h"

class ReviveScroll : public Item {
public:
	ReviveScroll(int price = 40);
	virtual ~ReviveScroll();
	virtual void Use(Character& target) override;
};

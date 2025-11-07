#pragma once

#include <memory>
#include <string>

#include "Item.h"

class ItemStack
{
public:
	ItemStack(std::shared_ptr<Item> prototype, int qty = 1);

	const std::string& GetName() const;
	int GetQuantity() const;

	std::shared_ptr<Item> GetPrototype() const;

	void Add(int amount);
	bool Remove(int amount);

private:
	std::shared_ptr<Item> m_itemPrototype;
	int m_quantity;
};


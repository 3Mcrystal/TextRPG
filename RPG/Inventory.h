#pragma once

#include <vector>
#include <memory>

#include "ItemStack.h"

class Inventory
{
public:

	Inventory();
	~Inventory();

	void AddItem(std::shared_ptr<Item> item, int quantity = 1);
	bool RemoveItem(const std::string& itemName, int quantity = 1);
	
	bool HasItem(const std::string& itemName) const;

	void UseItem(const std::string& itemName, class Character& target);

	void PrintInventory() const;

	const std::vector<ItemStack>& GetItemStacks() const;

private:
	std::vector<ItemStack> m_stacks;

	ItemStack* FindStack(const std::string& name);

};


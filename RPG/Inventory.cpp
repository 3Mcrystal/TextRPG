#include <iostream>

#include "Inventory.h"


Inventory::Inventory() {}
Inventory::~Inventory() {}

ItemStack* Inventory::FindStack(const std::string& name) {
	for (auto& s : m_stacks) {
		if (s.GetName() == name) {
			return &s;
		}
	}
	return nullptr;
}

void Inventory::AddItem(std::shared_ptr<Item> item, int amount) {
	auto* stack = FindStack(item->GetName());
	if (stack) {
		stack->Add(amount);
	} else {
		m_stacks.emplace_back(item, amount);
	}
}

bool Inventory::RemoveItem(const std::string& itemName, int amount) {
	for (size_t i = 0; i < m_stacks.size(); ++i) {
		if (m_stacks[i].GetName() == itemName) {
			if (!m_stacks[i].Remove(amount)) return false;

			if (m_stacks[i].GetQuantity() <= 0)
				m_stacks.erase(m_stacks.begin() + i);

			return true;
		}
	}
	return false;
}

bool Inventory::HasItem(const std::string& itemName) const {
	for (const auto& s : m_stacks) {
		if (s.GetName() == itemName) {
			return true;
		}
	}
	return false;
}

void Inventory::UseItem(const std::string& itemName, Character& target) {
	for (auto& stack : m_stacks) {
		if (stack.GetName() == itemName && stack.GetQuantity() > 0) {
			stack.GetPrototype()->Use(target);
			RemoveItem(itemName, 1);
			return;
		}
	}
	std::cout << "You don't have " << itemName << " in your inventory.\n";
}

void Inventory::PrintInventory() const {
	std::cout << "Inventory:\n";
	if (m_stacks.empty()) {
		std::cout << " (empty)\n";
		return;
	}
	int i = 0;
	for (const auto& s : m_stacks) {
		std::cout << "[" << i++ << "] " << s.GetName() << " x" << s.GetQuantity() << "\n";
	}
}

const std::vector<ItemStack>& Inventory::GetItemStacks() const {
	return m_stacks;
}
#include <iostream>

#include "Merchant.h"
#include "PlayerParty.h"
#include "Potion.h"
#include "Antidote.h"
#include "SmokeBomb.h"
#include "InputManager.h"
#include "Character.h"

Merchant::Merchant() {
	m_stock.AddItem(std::make_shared<Potion>(),    3);  // 5 gold, heals 20
	m_stock.AddItem(std::make_shared<Antidote>(),  2);  // 12 gold, cures curse
	m_stock.AddItem(std::make_shared<SmokeBomb>(), 2);  // 8 gold, guaranteed escape
}

Merchant::~Merchant() {}

void Merchant::Interact(PlayerParty& party, InputManager& input) {

	bool hasCurse = false;
	for (auto& m : party.GetMembers())
		if (m->IsCursed()) { hasCurse = true; break; }

	m_priceMultiplier = hasCurse ? 4.0f : 1.0f;

	if (hasCurse)
		std::cout << "\"Heh heh... rough day, stranger?\"\n";
	else
		std::cout << "\"What're ya buyin'?\"\n";

	bool done = false;
	while (!done) {
		std::cout << "\nYour gold: " << party.GetGold() << "\n";
		std::cout << "----  Merchant Stock  ----\n";

		auto& stacks = m_stock.GetItemStacks();
		if (stacks.empty()) {
			std::cout << "Out of stock!\n";
		} else {
			int idx = 0;
			for (auto& s : stacks) {
				int finalPrice = (int)(s.GetPrototype()->GetPrice() * m_priceMultiplier);
				std::cout << "[" << idx++ << "] "
				          << s.GetPrototype()->GetName()
				          << "  -  " << finalPrice << " Gold"
				          << "  (stock: " << s.GetQuantity() << ")\n";
			}
		}
		std::cout << "[" << stacks.size() << "] Leave\n> ";

		std::string choice = input.GetLine();
		int index = -1;
		try { index = std::stoi(choice); } catch (...) { index = -1; }

		if (index == (int)stacks.size()) {
			std::cout << "\"Come back anytime!\"\n";
			done = true;
			continue;
		}

		if (index >= 0 && index < (int)stacks.size()) {
			auto& stack = stacks[index];
			int price = (int)(stack.GetPrototype()->GetPrice() * m_priceMultiplier);

			if (party.GetGold() < price) {
				std::cout << "\"Not enough gold, stranger!\"\n";
				continue;
			}

			party.AddGold(-price);
			party.GetInventory().AddItem(stack.GetPrototype(), 1);
			m_stock.RemoveItem(stack.GetName(), 1);
			std::cout << "Bought " << stack.GetName() << " for " << price << " Gold.\n";
		} else {
			std::cout << "Invalid choice.\n";
		}
	}
}

#include <iostream>

#include "Merchant.h"
#include "PlayerParty.h"
#include "Potion.h"
#include "InputManager.h"
#include "Character.h"

Merchant::Merchant() {
    // Restock at each encounter
    m_stock.AddItem(std::make_shared<Potion>(), 3);
}

Merchant::~Merchant() {}

void Merchant::Interact(PlayerParty& party, InputManager& input) {

    bool hasCurse = false;
    for (auto& m : party.GetMembers()) {
        if (m->IsCursed()) {
            hasCurse = true;
            break;
        }
	}

	// If cursed, higher prices
	m_priceMultiplier = hasCurse ? 4.0f : 1.0f;

    if (hasCurse) {
        std::cout << "Heh heh. Having a rough day, stranger?\n";
    }
    else {
        std::cout << "What're ya buyin'?\n";
    }

    bool done = false;

    while (!done) {

        std::cout << "Your gold: " << party.GetGold() << "\n\n";
        std::cout << "\nMerchant Stock\n";
        auto& stacks = m_stock.GetItemStacks();

        if (stacks.empty()) {
            std::cout << "Out of stock!\n";
        } else {
            int idx = 0;
            for (auto& s : stacks) {

				int basePrice = s.GetPrototype()->GetPrice();
				int finalPrice = (int)basePrice * m_priceMultiplier;

                std::cout << "[" << idx++ << "] " << s.GetPrototype()->GetName() << " - Price: " << finalPrice << " Gold" << " | Stock: " << s.GetQuantity() << "\n";

            }
        }

        std::cout << "[" << stacks.size() << "] Exit Merchant\n> ";

        std::string choice = input.GetLine();
        int index = -1;

        try {
            index = std::stoi(choice);
        }
        catch (...) {
            index = -1;
        }

        if (index == (int)stacks.size()) {
            std::cout << "Come back anytime!\n";
            done = true;
            continue;
        }

        if (index >= 0 && index < (int)stacks.size()) {
            auto& stack = stacks[index];
            int price = (int)stack.GetPrototype()->GetPrice() * m_priceMultiplier;


            if (party.GetGold() < price) {
                std::cout << "Not enough cash, stranger!\n";
                continue;
            }

            // Buy the item
            party.AddGold(-price);
            party.GetInventory().AddItem(stack.GetPrototype(), 1);
            m_stock.RemoveItem(stack.GetName(), 1);

            std::cout << "You bought a " << stack.GetName() << " for " << price << " Gold.\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

#include <iostream>

#include "Merchant.h"
#include "PlayerParty.h"
#include "Potion.h"
#include "InputManager.h"

Merchant::Merchant() {
    // Restock at each encounter
    m_stock.AddItem(std::make_shared<Potion>(), 3);
}

Merchant::~Merchant() {}

void Merchant::Interact(PlayerParty& party, InputManager& input) {
    std::cout << "A merchant approaches.\n";
    std::cout << "What're ya buyin'?\n";

    bool done = false;

    while (!done) {

        std::cout << "\n=== Merchant Stock ===\n";
        auto& stacks = m_stock.GetItemStacks();

        if (stacks.empty()) {
            std::cout << "Out of stock!\n";
        } else {
            int idx = 0;
            for (auto& s : stacks) {
                std::cout << "[" << idx++ << "] " << s.GetName() << " - Price: " << s.GetPrototype()->GetPrice() << " Gold" << " | Stock: " << s.GetQuantity() << "\n";
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
            int price = stack.GetPrototype()->GetPrice();

            if (party.GetGold() < price) {
                std::cout << "Not enough cash, stranger!\n";
                continue;
            }

            // Buy the item
            party.AddGold(-price);
            party.GetInventory().AddItem(stack.GetPrototype(), 1);
            m_stock.RemoveItem(stack.GetName(), 1);

            std::cout << "You bought a " << stack.GetName()
                      << " for " << price << " Gold.\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

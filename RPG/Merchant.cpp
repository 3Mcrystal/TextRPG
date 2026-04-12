#include <iostream>
#include <random>
#include <vector>
#include <string>
#include "Merchant.h"
#include "PlayerParty.h"
#include "Potion.h"
#include "Antidote.h"
#include "SmokeBomb.h"
#include "ReviveScroll.h"
#include "InputManager.h"
#include "Character.h"

//Yes, they're quote as resident evil 4 merchant
//Okay, almost all of them
static std::string RandomGreeting(bool hasCurse) {
    static std::random_device rd; static std::mt19937 gen(rd());
    if (hasCurse) {
        static const std::vector<std::string> cursedQuotes = {
            "\"Heh heh. Having a rough day, stranger?\"",
            "\"That cash in your pocket or your life? Easy choice, mate.\"",
            "\"You have the stench of battle on you, mate. Heh heh.\"",
			"\"Your valuables won't do you much good in the grave.\"",
        };
        std::uniform_int_distribution<> d(0, (int)cursedQuotes.size()-1);
        return cursedQuotes[d(gen)];
    }
    static const std::vector<std::string> greetings = {
        "\"What're ya buyin'?\"",
        "\"Got somethin' that might interest ya'!.\"",
        "\"Got some rare things on sale, stranger!\"",
        "\"Welcome!\"",
        "\"Let's do some business then, eh!\"",
    };
    std::uniform_int_distribution<> d(0, (int)greetings.size()-1);
    return greetings[d(gen)];
}

static std::string RandomPurchaseQuote() {
    static std::random_device rd; static std::mt19937 gen(rd());
    static const std::vector<std::string> quotes = {
        "\"Anything else I can 'elp you with?\"",
        "\"*laughs* Thank you!\"",
        "\"Is that all, stranger?\"",
        "\"Thank you!\"",
        "\"Will that be all, then?\"",
    };
    std::uniform_int_distribution<> d(0, (int)quotes.size()-1);
    return quotes[d(gen)];
}

static std::string RandomBrokeQuote() {
    static std::random_device rd; static std::mt19937 gen(rd());
    static const std::vector<std::string> quotes = {
        "\"Come back when you can actually pay, mate.\"",
        "\"This ain't a charity!\"",
        "\"That won't cover it, I'm afraid.\"",
        "\"Save up, then we can talk.\"",
    };
    std::uniform_int_distribution<> d(0, (int)quotes.size()-1);
    return quotes[d(gen)];
}

static std::string RandomFarewellQuote() {
    static std::random_device rd; static std::mt19937 gen(rd());
    static const std::vector<std::string> quotes = {
        "\"Heh heh heh... I'll be seein' ya.\"",
        "\"Until next time, stranger.\"",
        "\"Now go and cause some mayhem, eh! Heh heh heh!\"",
        "\"Come back anytime.\"",
		"\"See you, stranger.\"",
    };
    std::uniform_int_distribution<> d(0, (int)quotes.size()-1);
    return quotes[d(gen)];
}

Merchant::Merchant() {
    m_stock.AddItem(std::make_shared<Potion>(),       3);  //5 gold
    m_stock.AddItem(std::make_shared<Antidote>(),     2);  //12 
    m_stock.AddItem(std::make_shared<SmokeBomb>(),    2);  //8
    m_stock.AddItem(std::make_shared<ReviveScroll>(), 1);  //40
}

Merchant::~Merchant() {}

void Merchant::Interact(PlayerParty& party, InputManager& input) {
    bool hasCurse = false;
    for (auto& m : party.GetMembers())
        if (m->IsCursed()) { hasCurse = true; break; }

    m_priceMultiplier = hasCurse ? 3.0f : 1.0f;

    std::cout << RandomGreeting(hasCurse) << "\n";

    bool done = false;
    while (!done) {
        std::cout << "\nYour gold: " << party.GetGold() << "\n";
        std::cout << "----  Merchant Stock  ----\n";
        auto& stacks = m_stock.GetItemStacks();

        if (stacks.empty()) {
            std::cout << "\"I don't always have this in stock, strange\"\n";
        } else {
            int idx = 0;
            for (auto& s : stacks) {
                int price = (int)(s.GetPrototype()->GetPrice() * m_priceMultiplier);
                std::cout << "[" << idx++ << "] "
                          << s.GetPrototype()->GetName()
                          << "  -  " << price << " Gold"
                          << "  (stock: " << s.GetQuantity() << ")\n";
            }
        }
        std::cout << "[" << stacks.size() << "] Leave\n> ";

        std::string choice = input.GetLine();
        int index = -1;
        try { index = std::stoi(choice); } catch (...) { index = -1; }

        if (index == (int)stacks.size()) {
            std::cout << RandomFarewellQuote() << "\n";
            done = true; continue;
        }
        if (index >= 0 && index < (int)stacks.size()) {
            auto& stack = stacks[index];
            int price = (int)(stack.GetPrototype()->GetPrice() * m_priceMultiplier);
            if (party.GetGold() < price) {
                std::cout << RandomBrokeQuote() << "\n"; continue;
            }
            party.AddGold(-price);
            party.GetInventory().AddItem(stack.GetPrototype(), 1);
            m_stock.RemoveItem(stack.GetName(), 1);
            std::cout << "Bought " << stack.GetName() << " for " << price << " Gold.\n";
            std::cout << RandomPurchaseQuote() << "\n";
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

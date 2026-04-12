#include <iostream>
#include <vector>
#include <memory>
#include "Dragon.h"

Dragon::Dragon(const std::string& name)
    : Character(name, 150, 18, 6, 0, 2) {}

Dragon::~Dragon() {}
void Dragon::Update() {}

void Dragon::DragonBreath(std::vector<std::shared_ptr<Character>>& targets) {
    std::cout << m_name << " INHALES deeply...\n";
    std::cout << "*** DRAGON BREATH — hits the entire party! ***\n";
    int dmg = 14;
    for (auto& t : targets) {
        if (!t->IsAlive()) continue;
        std::cout << "  " << t->GetName() << " takes " << dmg << " fire damage!\n";
        t->TakeDamage(dmg);
        //Burn (3 turns, 5 dmg/turn)
        t->ApplyStatus(StatusType::Burn, 3, 5);
    }
}

void Dragon::TailSwipe(Character& target) {
    int dmg = GetAttack() + 8;
    std::cout << m_name << " TAIL SWIPES " << target.GetName()
              << " for " << dmg << " damage!\n";
    target.TakeDamage(dmg);
}

int Dragon::GetXpReward()   const { return 200; }
int Dragon::GetGoldReward() const { return 100; }

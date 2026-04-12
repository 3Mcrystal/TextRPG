#include <iostream>
#include "ReviveScroll.h"
#include "Character.h"

ReviveScroll::ReviveScroll(int price)
	: Item("Revive Scroll", price) {}

ReviveScroll::~ReviveScroll() {}

void ReviveScroll::Use(Character& target) {
	if (target.IsAlive()) {
		std::cout << target.GetName() << " is already alive. The scroll crumbles unused.\n";
		return;
	}
	// Revive with 25% HP
	int reviveHp = std::max(1, target.GetMaxHp() / 4);
	target.SetHp(reviveHp);
	std::cout << "A golden light erupts from the scroll!\n";
	std::cout << target.GetName() << " is revived with " << reviveHp << " HP!\n";
}

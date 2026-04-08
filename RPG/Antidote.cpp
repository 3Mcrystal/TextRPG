#include <iostream>
#include "Antidote.h"
#include "Character.h"

Antidote::Antidote(int price)
	: Item("Antidote", price) {}

Antidote::~Antidote() {}

void Antidote::Use(Character& target)
{
	if (target.IsCursed()) {
		target.RemoveCurse();
		std::cout << target.GetName() << "'s curse has been lifted!\n";
	} else {
		std::cout << target.GetName() << " is not cursed. The Antidote fizzles...\n";
	}
}

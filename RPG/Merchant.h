#pragma once

#include "Inventory.h"

class PlayerParty;
class InputManager;

class Merchant
{
public:
	Merchant();
	~Merchant();

	void Interact(PlayerParty& party, InputManager& input);

	Inventory& GetStock() { return m_stock; }

private:
	Inventory m_stock; // Merchant's inventory
};


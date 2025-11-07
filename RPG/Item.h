#pragma once

#include <string>

class Character;

class Item
{
public:
	Item(const std::string& name, int price);
	virtual ~Item();

	std::string GetName() const;
	int GetPrice() const;

	virtual void Use(Character& target) = 0;

protected:
	std::string m_name;
	int m_price;
};


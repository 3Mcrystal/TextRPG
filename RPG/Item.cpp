#include "Item.h"

Item::Item(const std::string& name, int price)
	: m_name(name), m_price(price) {}

Item::~Item() {}

std::string Item::GetName() const {return m_name;}
int Item::GetPrice() const { return m_price; }

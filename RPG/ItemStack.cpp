#include "ItemStack.h"

ItemStack::ItemStack(std::shared_ptr<Item> prototype, int qty)
    : m_itemPrototype(prototype), m_quantity(qty)
    , m_name(prototype->GetName())
{}

const std::string& ItemStack::GetName() const {
    return m_name;
}

int ItemStack::GetQuantity() const {
    return m_quantity;
}

std::shared_ptr<Item> ItemStack::GetPrototype() const {
    return m_itemPrototype;
}

void ItemStack::Add(int amount) {
    m_quantity += amount;
}

bool ItemStack::Remove(int amount) {
    if (m_quantity < amount) return false;
    m_quantity -= amount;
    return true;
}

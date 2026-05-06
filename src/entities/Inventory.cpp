#include "Inventory.h"

void Inventory::init() {
    items.clear();
}

void Inventory::addResource(MaterialType type, int amount) {
    items[type] += amount;
}

bool Inventory::hasResource(MaterialType type, int amount) const {
    auto it = items.find(type);
    if (it != items.end()) {
        return it->second >= amount;
    }
    return false;
}

int Inventory::getAmount(MaterialType type) const {
    auto it = items.find(type);
    if (it != items.end()) {
        return it->second;
    }
    return 0;
}

void Inventory::clear() {
    items.clear();
}
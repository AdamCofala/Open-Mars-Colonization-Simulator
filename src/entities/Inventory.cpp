#include "Inventory.h"

void Inventory::init(int capacity) {
    maxCapacity = capacity;
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

int Inventory::getMaxCapacity() const {
    return maxCapacity;
}

int Inventory::getCurrentLoad() const {
    int load = 0;
    for (const auto& pair : items) {
        load += pair.second;
    }
    return load;
}

void Inventory::clear() {
    items.clear();
}
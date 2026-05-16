#include "Inventory.h"

void Inventory::init() {
    items.clear();
    maxCapacities.clear();
}

void Inventory::setMaxCapacity(MaterialType type, int capacity) {
    maxCapacities[type] = capacity;
    if (items.find(type) == items.end()) {
        items[type] = 0;
    }
}

int Inventory::getMaxCapacity(MaterialType type) const {
    auto it = maxCapacities.find(type);
    if (it != maxCapacities.end()) {
        return it->second;
    }
    return 0;
}

bool Inventory::addResource(MaterialType type, int amount) {
    if (amount < 0) {
        items[type] += amount;
        return true;
    }

    int current = getAmount(type);
    int max = getMaxCapacity(type);

    if (current + amount > max) {
        return false;
    }

    items[type] += amount;
    return true;
}

bool Inventory::subResource(MaterialType type, int amount) {
    if (amount <= 0) return false;

    if (!hasResource(type, amount)) {
        return false;
    }

    return addResource(type, -amount);
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
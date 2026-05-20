#include "Inventory.h"

void Inventory::init() {
    items.clear();
    maxCapacities.clear();
}

void Inventory::setMaxCapacity(MaterialType type, float capacity) {
    maxCapacities[type] = capacity;
    if (items.find(type) == items.end()) {
        items[type] = 0.0f;
    }
}

float Inventory::getMaxCapacity(MaterialType type) const {
    auto it = maxCapacities.find(type);
    if (it != maxCapacities.end()) {
        return it->second;
    }
    return 0.0f;
}

bool Inventory::addResource(MaterialType type, float amount) {
    if (amount < 0.0f) {
        items[type] += amount;
        return true;
    }

    float current = getAmount(type);
    float max = getMaxCapacity(type);

    if (max > 0.0f && current + amount > max) {
        return false;
    }

    items[type] += amount;
    return true;
}

bool Inventory::subResource(MaterialType type, float amount) {
    if (amount <= 0.0f) return false;

    if (!hasResource(type, amount)) {
        return false;
    }

    return addResource(type, -amount);
}

bool Inventory::hasResource(MaterialType type, float amount) const {
    auto it = items.find(type);
    if (it != items.end()) {
        return it->second >= amount;
    }
    return false;
}

float Inventory::getAmount(MaterialType type) const {
    auto it = items.find(type);
    if (it != items.end()) {
        return it->second;
    }
    return 0.0f;
}

void Inventory::clear() {
    items.clear();
}
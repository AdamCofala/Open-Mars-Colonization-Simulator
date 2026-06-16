#include "Inventory.h"
#include <fstream>

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

void Inventory::save(std::ofstream& file) const {
    size_t itemsSize = items.size();
    file.write((char*)&itemsSize, sizeof(itemsSize));

    for (const auto& [type, amount] : items) {
        file.write((char*)&type, sizeof(type));
        file.write((char*)&amount, sizeof(amount));
    }

    size_t capSize = maxCapacities.size();
    file.write((char*)&capSize, sizeof(capSize));

    for (const auto& [type, cap] : maxCapacities) {
        file.write((char*)&type, sizeof(type));
        file.write((char*)&cap, sizeof(cap));
    }
}

void Inventory::load(std::ifstream& file) {
    size_t itemsSize;
    file.read((char*)&itemsSize, sizeof(itemsSize));

    items.clear();

    for (size_t i = 0; i < itemsSize; i++) {
        MaterialType type;
        float amount;

        file.read((char*)&type, sizeof(type));
        file.read((char*)&amount, sizeof(amount));

        items[type] = amount;
    }

    size_t capSize;
    file.read((char*)&capSize, sizeof(capSize));

    maxCapacities.clear();

    for (size_t i = 0; i < capSize; i++) {
        MaterialType type;
        float cap;

        file.read((char*)&type, sizeof(type));
        file.read((char*)&cap, sizeof(cap));

        maxCapacities[type] = cap;
    }
}

void Inventory::clear() {
    items.clear();
}
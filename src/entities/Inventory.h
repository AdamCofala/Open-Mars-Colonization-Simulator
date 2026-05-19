#pragma once
#include <unordered_map>
#include "MaterialType.h"

class Inventory {
private:
    std::unordered_map<MaterialType, int> items;
    std::unordered_map<MaterialType, int> maxCapacities;

public:
    void init();

    void setMaxCapacity(MaterialType type, int capacity);
    int getMaxCapacity(MaterialType type) const;

    bool addResource(MaterialType type, int amount); 
    bool subResource(MaterialType type, int amount);     
    bool hasResource(MaterialType type, int amount) const;
    int getAmount(MaterialType type) const;

    void clear();
};
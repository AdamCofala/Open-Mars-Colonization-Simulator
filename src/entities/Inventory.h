#pragma once
#include <unordered_map>
#include "MaterialType.h"

class Inventory {
private:
    std::unordered_map<MaterialType, float> items;
    std::unordered_map<MaterialType, float> maxCapacities;

public:
    void init();

    void setMaxCapacity(MaterialType type, float capacity);
    float getMaxCapacity(MaterialType type) const;

    bool addResource(MaterialType type, float amount); 
    bool subResource(MaterialType type, float amount);     
    bool hasResource(MaterialType type, float amount) const;
    float getAmount(MaterialType type) const;

    void clear();
};
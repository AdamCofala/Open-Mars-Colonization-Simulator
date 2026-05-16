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

    bool addResource(MaterialType type, int amount); // Change to bool if we want to check capacity!
	bool subResource(MaterialType type, int amount); // TODO : implement this using addResource with negative amount, also check if the amount to subtract is available before, return true if the operation was successful, false otherwise
    
    bool hasResource(MaterialType type, int amount) const;
    int getAmount(MaterialType type) const;

    void clear();
};
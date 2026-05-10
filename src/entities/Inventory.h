#pragma once
#include <unordered_map>
#include "MaterialType.h"

class Inventory {
private:
    std::unordered_map<MaterialType, int> items;

public:
    void init();

    void addResource(MaterialType type, int amount); // Change to bool if we want to check capacity!
	bool subResource(MaterialType type, int amount); // TODO : implement this using addResource with negative amount, also check if the amount to subtract is available before, return true if the operation was successful, false otherwise
    bool hasResource(MaterialType type, int amount) const;

    int getAmount(MaterialType type) const;

    void clear();
};
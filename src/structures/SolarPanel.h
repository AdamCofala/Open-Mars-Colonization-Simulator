#pragma once
#include "Structure.h"

// Structures are so simple that there is no need of creating separate .cpp files for each of them, we can just implement them in the header file.

class SolarPanel : public Structure {
public:
    SolarPanel() = default;

    void update(Inventory& globalInventory) override
    {
		globalInventory.addResource(MaterialType::Energy, 1);     
    }

};
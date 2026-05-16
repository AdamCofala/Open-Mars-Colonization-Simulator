#pragma once
#include "Structure.h"

class SolarPanel : public Structure {
public:
    SolarPanel() = default;

    void update(Inventory& globalInventory) override
    {
		globalInventory.addResource(MaterialType::Energy, 1);     
    }

};
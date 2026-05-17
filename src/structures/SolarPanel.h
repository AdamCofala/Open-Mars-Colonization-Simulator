#pragma once
#include "Structure.h"

class SolarPanel : public Structure {
public:
    SolarPanel() {
        productionRates[MaterialType::Energy] = 1;
    }

    void update(Inventory& globalInventory) override {
        Structure::update(globalInventory);
    }
};
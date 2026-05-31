#pragma once
#include "entities/Structure.h"
#include "entities/MaterialType.h"

class SolarPanel : public Structure {
	std::string textureId = "solar_panels";
public:
    SolarPanel(int x, int y) {
        productionRates[MaterialType::ENERGY] = 1;
        // Use the init method to set xOffset and yOffset since they are private in Structure
        init(x, y, "", 3, 3); // Provide default values for x, y, textureId, xOffset, yOffset
        setInternalCapacity(MaterialType::ENERGY, 500.0f);
    }

    void update(float dt, Map& map) override {
        Structure::update(dt, map);
    }
};
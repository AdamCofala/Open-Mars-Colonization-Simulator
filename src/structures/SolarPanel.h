#pragma once
#include "entities/Structure.h"
#include "entities/Enums.h"

class SolarPanel : public Structure {
public:
    SolarPanel(int x, int y) {
        init(x, y, "solar_panels", 3, 3);
        setOutputCapacity(MaterialType::ENERGY, 500.0f);

        StructurePort outputPort;
        outputPort.offsetX = 0;
        outputPort.offsetY = 0;
        outputPort.type = PortType::OUTPUT;
        outputPort.material = MaterialType::ENERGY;
        outputPort.dir = Direction::SOUTH_WEST;
        m_ports.push_back(outputPort);
    }

    StructureType getType() const override {
        return StructureType::SolarPanel;
    }

    float getProductionRate(MaterialType type, Map& map) const override {
        if (type == MaterialType::ENERGY) {
            int level = map.getTile(getX(), getY()).getLevel();
            return 1.0f + level * 0.5f; 
        }
        return 0.0f;
    }

    std::vector<MaterialType> getProducedMaterials() const override {
        return { MaterialType::ENERGY };
    }

    void update(float dt, Map& map) override {
        Structure::update(dt, map);
    }
};
#pragma once
#include "entities/Structure.h"
#include "entities/Enums.h"
class SolarPanel : public Structure {
public:
    SolarPanel(int x, int y) {
        productionRates[MaterialType::ENERGY] = 1;

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
    void update(float dt, Map& map) override {
        Structure::update(dt, map);
    }
};
#pragma once
#include "entities/Structure.h"
#include "entities/Enums.h"

class IceMelter : public Structure {
public:
    IceMelter(int x, int y) {
        productionRates[MaterialType::WATER] = 1;
        consumeRates[MaterialType::ENERGY] = 5;

        init(x, y, "ice_melter", 3, 3);
        setInternalCapacity(MaterialType::ENERGY, 500.0f);
        setInternalCapacity(MaterialType::WATER, 500.0f);

        StructurePort waterOutPort;
        waterOutPort.offsetX = 0;
        waterOutPort.offsetY = 0;
        waterOutPort.type = PortType::OUTPUT;
        waterOutPort.dir = Direction::SOUTH_WEST;

        m_ports.push_back(waterOutPort);

        StructurePort energyInPort;
        energyInPort.offsetX = 0;
        energyInPort.offsetY = -2;
        energyInPort.type = PortType::INPUT;
        energyInPort.dir = Direction::SOUTH_EAST;

        m_ports.push_back(energyInPort);
    }

    void update(float dt, Map& map) override {
        Structure::update(dt, map);
    }
};
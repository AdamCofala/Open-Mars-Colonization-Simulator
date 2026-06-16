#pragma once
#include "entities/Structure.h"
#include "entities/Enums.h"
#include <algorithm>

class WaterMagazine : public Structure {
public:
    WaterMagazine(int x, int y) {
        // Czysty magazyn – brak produkcji / konsumpcji
        init(x, y, "water_magazine", 2, 1);

        setInputCapacity(MaterialType::WATER, 250.0f);
        setOutputCapacity(MaterialType::WATER, 250.0f);

        // Port INPUT na górnym tile'u (offsetX = -1), kierunek zachodni
        StructurePort waterInPort;
        waterInPort.offsetX = -1;
        waterInPort.offsetY = 0;
        waterInPort.type = PortType::INPUT;
        waterInPort.material = MaterialType::WATER;
        waterInPort.dir = Direction::NORTH_WEST;   // zachód
        m_ports.push_back(waterInPort);

        // Port OUTPUT na przednim tile'u (offsetX = 0), kierunek wschodni
        StructurePort waterOutPort;
        waterOutPort.offsetX = 0;
        waterOutPort.offsetY = 0;
        waterOutPort.type = PortType::OUTPUT;
        waterOutPort.material = MaterialType::WATER;
        waterOutPort.dir = Direction::SOUTH_EAST;  // wschód
        m_ports.push_back(waterOutPort);
    }

    void update(float dt, Map& map) override {
        // Transfer wody z input do output inventory (magazyn nie produkuje sam)
        float waterInInput = getInputInventory().getAmount(MaterialType::WATER);
        if (waterInInput > 0.0f) {
            float spaceInOutput = getOutputInventory().getMaxCapacity(MaterialType::WATER)
                - getOutputInventory().getAmount(MaterialType::WATER);
            float transferAmount = std::min(waterInInput, spaceInOutput);
            if (transferAmount > 0.0f) {
                getInputInventory().subResource(MaterialType::WATER, transferAmount);
                getOutputInventory().addResource(MaterialType::WATER, transferAmount);
            }
        }
    }
};
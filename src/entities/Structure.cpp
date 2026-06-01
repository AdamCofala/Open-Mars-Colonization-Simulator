#include "Structure.h"

void Structure::init(int startX, int startY, const std::string& texId, int xOffset, int yOffset)
{
	x = startX;
	y = startY;
	textureId = texId;
	this->xOffset = xOffset;
	this->yOffset = yOffset;
   internalInventory.init();
}

void Structure::setInternalCapacity(MaterialType type, float capacity) {
    internalInventory.setMaxCapacity(type, capacity);
}

void Structure::update(float dt, Map& map) {
    bool canOperate = true;

    for (const auto& [material, rate] : consumeRates) {
        float needed = rate * dt;
        if (needed > 0.0f && !internalInventory.hasResource(material, needed)) {
            canOperate = false;
            break;
        }
    }

    if (canOperate) {
        for (const auto& [material, rate] : consumeRates) {
            float needed = rate * dt;
            if (needed > 0.0f) {
                internalInventory.subResource(material, needed);
            }
        }

        for (const auto& [material, rate] : productionRates) {
            float produced = rate * dt;
            if (produced > 0.0f) {
                internalInventory.addResource(material, produced);
            }
        }
    }
}

const Inventory& Structure::getInternalInventory() const {
    return internalInventory;
}

std::string Structure::getTextureId() const {
    return textureId;
}

int Structure::getX() const {
    return x;
}

int Structure::getY() const {
    return y;
}

bool Structure::isPipe() const {
    return false;
}

const std::vector<StructurePort>& Structure::getPorts() const {
    return m_ports;
}

PortType Structure::getPortAtTile(int worldX, int worldY, Direction edgeDir) const {
    for (const auto& port : m_ports) {
        int portWorldX = x + port.offsetX;
        int portWorldY = y + port.offsetY;

        if (portWorldX == worldX && portWorldY == worldY && port.dir == edgeDir) {
            return port.type;
        }
    }
    return PortType::NONE;
}
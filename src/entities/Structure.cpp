#include "Structure.h"

void Structure::init(int startX, int startY, const std::string& texId, int xOffset, int yOffset)
{
	x = startX;
	y = startY;
	textureId = texId;
	this->xOffset = xOffset;
	this->yOffset = yOffset;
    inputInventory.init();
    outputInventory.init();
}

void Structure::setInputCapacity(MaterialType type, float capacity) {
    inputInventory.setMaxCapacity(type, capacity);
}
void Structure::setOutputCapacity(MaterialType type, float capacity) {
    outputInventory.setMaxCapacity(type, capacity);
}

void Structure::update(float dt, Map& map) {
    bool canOperate = true;

    for (const auto& [material, rate] : consumeRates) {
        float needed = rate * dt;
        if (needed > 0.0f && !inputInventory.hasResource(material, needed)) {
            canOperate = false;
            break;
        }
    }

    if (canOperate) {
        for (const auto& [material, rate] : consumeRates) {
            float needed = rate * dt;
            if (needed > 0.0f)
                inputInventory.subResource(material, needed);
        }
        for (const auto& [material, rate] : productionRates) {
            float produced = rate * dt;
            if (produced > 0.0f)
                outputInventory.addResource(material, produced);
        }
    }
}

MaterialType Structure::getMaterialAtPort(int pipeWorldX, int pipeWorldY, Direction edgeDir) const {
    for (const auto& port : m_ports) {
        int portX = this->x + port.offsetX;
        int portY = this->y + port.offsetY;
        if (port.dir == Direction::NORTH_EAST) { portY -= 1; }
        else if (port.dir == Direction::SOUTH_EAST) { portX += 1; }
        else if (port.dir == Direction::SOUTH_WEST) { portY += 1; }
        else if (port.dir == Direction::NORTH_WEST) { portX -= 1; }
        if (portX == pipeWorldX && portY == pipeWorldY && port.dir == edgeDir)
            return port.material;
    }
    return MaterialType::NONE;
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

PortType Structure::getPortAtTile(int pipeWorldX, int pipeWorldY, Direction edgeDir) const {
    for (const auto& port : m_ports) {
        int portX = this->x + port.offsetX;
        int portY = this->y + port.offsetY;

        if (port.dir == Direction::NORTH_EAST) { portY -= 1; }
        else if (port.dir == Direction::SOUTH_EAST) { portX += 1; }
        else if (port.dir == Direction::SOUTH_WEST) { portY += 1; }
        else if (port.dir == Direction::NORTH_WEST) { portX -= 1; }

        if (portX == pipeWorldX && portY == pipeWorldY && port.dir == edgeDir) {
            return port.type;
        }
    }
    return PortType::NONE;
}

void Structure::save(std::ofstream& file) const {
    file.write((char*)&x, sizeof(x));
    file.write((char*)&y, sizeof(y));

    file.write((char*)&xOffset, sizeof(xOffset));
    file.write((char*)&yOffset, sizeof(yOffset));

    size_t len = textureId.size();
    file.write((char*)&len, sizeof(len));
    file.write(textureId.c_str(), len);

    inputInventory.save(file);
    outputInventory.save(file);
}

void Structure::load(std::ifstream& file) {
    file.read((char*)&x, sizeof(x));
    file.read((char*)&y, sizeof(y));

    file.read((char*)&xOffset, sizeof(xOffset));
    file.read((char*)&yOffset, sizeof(yOffset));

    size_t len;
    file.read((char*)&len, sizeof(len));

    textureId.resize(len);
    file.read(&textureId[0], len);

    inputInventory.load(file);
    outputInventory.load(file);
}
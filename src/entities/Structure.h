#pragma once
#include <string>
#include <unordered_map>
#include "Inventory.h"
#include "Enums.h"

// Stuctures need to output / input ratio, u can't just update global inventory
// like that, you need to have some internal state that tracks how much resources the
// structure has produced / consumed per internal time, and then update the global inventory based on that.
// This way you can also implement structures that consume resources, like a factory that consumes energy to produce goods,
// or a water pump that consumes energy to produce water.

class Map;

class Structure {
protected:
    std::unordered_map<MaterialType, int> productionRates; // How much of each resource the structure produces per internal time unit
    std::unordered_map<MaterialType, int> consumeRates;
    std::vector<StructurePort> m_ports;

    void setInternalCapacity(MaterialType type, float capacity);
private:
    int x;
    int y;
    std::string textureId;
    Inventory inputInventory;
    Inventory outputInventory;

    int xOffset; // How many tiles the structure occupies in x direction
    int yOffset; // How many tiles the structure occupies in y direction

public:
    virtual ~Structure() = default;

    virtual void init(int startX, int startY, const std::string& texId, int xOffset, int yOffset);
    virtual void update(float dt, Map& map);
    virtual std::string getTextureId() const;

    int getX() const;
    int getY() const;
    int getXOffset() const { return xOffset; }
    int getYOffset() const { return yOffset; }

    Inventory& getInputInventory() { return inputInventory; }
    Inventory& getOutputInventory() { return outputInventory; }
    const Inventory& getInputInventory()  const { return inputInventory; }
    const Inventory& getOutputInventory() const { return outputInventory; }

    void setInputCapacity(MaterialType type, float capacity);
    void setOutputCapacity(MaterialType type, float capacity);

    virtual bool isPipe() const;
    const std::vector<StructurePort>& getPorts() const;
    PortType getPortAtTile(int worldX, int worldY, Direction edgeDir) const;
    MaterialType getMaterialAtPort(int worldX, int worldY, Direction edgeDir) const;
};

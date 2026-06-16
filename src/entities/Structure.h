#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Inventory.h"
#include "Enums.h"

class Map;

class Structure {
protected:
    std::unordered_map<MaterialType, int> productionRates;
    std::unordered_map<MaterialType, int> consumeRates;
    std::vector<StructurePort> m_ports;

    void setInternalCapacity(MaterialType type, float capacity);

    // Nowe metody wirtualne – domyœlnie korzystaj¹ z map, ale klasy pochodne mog¹ je przes³oniæ
    virtual float getProductionRate(MaterialType type, Map& map) const;
    virtual float getConsumptionRate(MaterialType type, Map& map) const;
    virtual std::vector<MaterialType> getProducedMaterials() const;
    virtual std::vector<MaterialType> getConsumedMaterials() const;

private:
    int x;
    int y;
    std::string textureId;
    Inventory inputInventory;
    Inventory outputInventory;
    int xOffset;
    int yOffset;

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

    virtual StructureType getType() const = 0;

    virtual void save(std::ofstream& file) const;
    virtual void load(std::ifstream& file);
};
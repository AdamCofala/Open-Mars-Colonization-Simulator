#pragma once
#include "Map.h"
#include "../entities/Inventory.h"
#include "WorldGenSettings.h"
#include <memory>

class World {
private:
    std::unique_ptr<Map> map;
    Inventory globalInventory;
    float timeAccumulator = 0.0f;
    const float secondsPerGameDay = 5.0f;
    float totalEnergy = 0.0f;
    float totalEnergyCapacity = 0.0f;
    float totalWater = 0.0f;
    float totalWaterCapacity = 0.0f;

    int day = 1;
    int month = 1;
    int year = 2050;

public:
    World() = default;
    ~World() = default;

    void init(int mapWidth = 128, int mapHeight = 128,
        const WorldGenSettings& settings = WorldGenSettings{});
    void update(float dt);
    void shutdown();
    Map& getMap() const;
    Inventory& getGlobalInventory() { return globalInventory; }
    const Inventory& getGlobalInventory() const { return globalInventory; }

    int   getDay()   const;
    int   getMonth() const;
    int   getYear()  const;
    float getTotalEnergy()         const { return totalEnergy; }
    float getTotalEnergyCapacity() const { return totalEnergyCapacity; }
    float getTotalWater()          const { return totalWater; }
    float getTotalWaterCapacity()  const { return totalWaterCapacity; }

    void save(const std::string& path);
    void load(const std::string& path);

    void saveToSlot(int slot);
    void loadFromSlot(int slot);
};
// TODO : Implement a World class that manages the game world, including entities, map, resource logic etc. !
#pragma once
#include "Map.h"
#include "../entities/Inventory.h"

class World {
private:
	Map* map = nullptr;
    Inventory globalInventory;
    float timeAccumulator = 0.0f;
    const float secondsPerGameDay = 5.0f;

    int day = 1;
    int month = 1;
    int year = 2050;

public:
	void init(int mapWidth = 191, int mapHeight = 191);
	void update(float dt);
	void shutdown();
	Map& getMap() const;

    int getDay() const;
    int getMonth() const;
    int getYear() const;
};
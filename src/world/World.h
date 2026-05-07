// TODO : Implement a World class that manages the game world, including entities, map, resource logic etc. !
#pragma once
#include "Map.h"

class World {
private:
	Map* map = nullptr;

public:
	void init(int mapWidth = 191, int mapHeight = 191);
	void update(float dt);
	void shutdown();
	Map& getMap() const;

};
#include "World.h"
#include <stdexcept>


void World::init(int mapWidth, int mapHeight) {

	if (map) {
		throw std::runtime_error("World is already initialized!");
	}

	map = new Map();
	map->init(mapWidth, mapHeight);
	map->generateTerrain();
}

void World::update(float dt) {
	// Update world logic here
}

void World::shutdown() {
	if (map) {
		//map->shutdown(); We should call it map has some resources to free, but currently it doesn't
		delete map;
		map = nullptr;
	}
}

Map& World::getMap() const {
	return *map;
}
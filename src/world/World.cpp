#include "World.h"
#include <stdexcept>


void World::init(int mapWidth, int mapHeight) {

	if (map) {
		throw std::runtime_error("World is already initialized!");
	}

	map = new Map();
	map->init(mapWidth, mapHeight);
	map->generateTerrain();

	globalInventory.init();

	timeAccumulator = 0.0f;
	day = 1;
	month = 1;
	year = 2050;
}

void World::update(float dt) {
	// Update world logic here
	timeAccumulator += dt;

  while (timeAccumulator >= secondsPerGameDay) {
		timeAccumulator -= secondsPerGameDay;
		day++;

		if (day > 30) {
			day = 1;
			month++;

			if (month > 12) {
				month = 1;
				year++;
			}
		}
	}

	if (map != nullptr) {
		auto& allStructures = map->getStructures();
		totalEnergy = 0.0f;
		totalEnergyCapacity = 0.0f;

		for (auto& structure : allStructures) {
			structure.update(dt);
           totalEnergy += structure.getInternalInventory().getAmount(MaterialType::ENERGY);
			totalEnergyCapacity += structure.getInternalInventory().getMaxCapacity(MaterialType::ENERGY);
		}
	}
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

int World::getDay() const {
	return day;
}

int World::getMonth() const {
	return month;
}

int World::getYear() const {
	return year;
}

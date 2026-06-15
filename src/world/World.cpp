#include "World.h"
#include <stdexcept>


void World::init(int mapWidth, int mapHeight, const WorldGenSettings& settings) {
	if (map) {
		throw std::runtime_error("World is already initialized!");
	}

	map = new Map();
	map->init(mapWidth, mapHeight);
	map->generateTerrain(settings);   // pass settings through

	globalInventory.init();

	timeAccumulator = 0.0f;
	day = 1;
	month = 1;
	year = 2050;
}

void World::update(float dt) {
	// Advance the global time logic and calculate in-game calendar progression
	timeAccumulator += dt;

	while (timeAccumulator >= secondsPerGameDay) {
		timeAccumulator -= secondsPerGameDay;
		day++;

		// 30 days per game month
		if (day > 30) {
			day = 1;
			month++;

			// 12 months per game year
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
		totalWater = 0.0f;
		totalWaterCapacity = 0.0f;

		// Calculate total energy produced and accumulated by structures
		for (auto& structure : allStructures) {
			if (structure != nullptr) { // Dobra praktyka: upewniamy si, e wskanik jest wany

				// 1. Wywoujemy update przekazujc dt ORAZ map (uywamy -> zamiast .)
				structure->update(dt, *map);

				// 2. Pobieramy dane z ekwipunku (rwnie przez ->)
				totalEnergy += structure->getInternalInventory().getAmount(MaterialType::ENERGY);
				totalEnergyCapacity += structure->getInternalInventory().getMaxCapacity(MaterialType::ENERGY);

				totalWater += structure->getInternalInventory().getAmount(MaterialType::WATER);
				totalWaterCapacity += structure->getInternalInventory().getMaxCapacity(MaterialType::WATER);
			}
		}

		map->updateNetworks(dt); // Flow resources through pipes
	}
}

void World::shutdown() {
	if (map) {
		// We should call map->shutdown() if the map has resources to free, but currently it doesn't
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

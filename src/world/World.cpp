#include "World.h"
#include <stdexcept>
#include <fstream>


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

		for (auto& structure : allStructures) {
			if (structure != nullptr) {

				structure->update(dt, *map);

				totalEnergy += structure->getInputInventory().getAmount(MaterialType::ENERGY)
					+ structure->getOutputInventory().getAmount(MaterialType::ENERGY);
				totalEnergyCapacity += structure->getInputInventory().getMaxCapacity(MaterialType::ENERGY)
					+ structure->getOutputInventory().getMaxCapacity(MaterialType::ENERGY);
				totalWater += structure->getInputInventory().getAmount(MaterialType::WATER)
					+ structure->getOutputInventory().getAmount(MaterialType::WATER);
				totalWaterCapacity += structure->getInputInventory().getMaxCapacity(MaterialType::WATER)
					+ structure->getOutputInventory().getMaxCapacity(MaterialType::WATER);
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

void World::save(const std::string& path) {
	std::ofstream file(path, std::ios::binary);
	if (!file) return;

	int version = 1;
	file.write((char*)&version, sizeof(version));

	file.write((char*)&day, sizeof(day));
	file.write((char*)&month, sizeof(month));
	file.write((char*)&year, sizeof(year));

	file.write((char*)&totalEnergy, sizeof(totalEnergy));
	file.write((char*)&totalEnergyCapacity, sizeof(totalEnergyCapacity));

	file.write((char*)&totalWater, sizeof(totalWater));
	file.write((char*)&totalWaterCapacity, sizeof(totalWaterCapacity));

	globalInventory.save(file);

	map->save(file);

	file.close();
}

void World::load(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) return;

	int version;
	file.read((char*)&version, sizeof(version));

	file.read((char*)&day, sizeof(day));
	file.read((char*)&month, sizeof(month));
	file.read((char*)&year, sizeof(year));

	file.read((char*)&totalEnergy, sizeof(totalEnergy));
	file.read((char*)&totalEnergyCapacity, sizeof(totalEnergyCapacity));

	file.read((char*)&totalWater, sizeof(totalWater));
	file.read((char*)&totalWaterCapacity, sizeof(totalWaterCapacity));

	globalInventory.load(file);

	map->load(file);

	file.close();
}

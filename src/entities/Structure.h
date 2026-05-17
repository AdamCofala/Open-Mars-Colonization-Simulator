#pragma once
#include <string>
#include <unordered_map>
#include "Inventory.h"\
#include "MaterialType.h"

// Stuctures need to output / input ratio, u can't just update global inventory
// like that, you need to have some internal state that tracks how much resources the
// structure has produced / consumed per internal time, and then update the global inventory based on that.
// This way you can also implement structures that consume resources, like a factory that consumes energy to produce goods,
// or a water pump that consumes energy to produce water.

class Structure {
private:
	int x;
	int y;
	std::string textureId;
	Inventory internalInventory; // Inventory for the structure to track its own resources, for example how much energy it has produced / consumed, how much water it has produced / consumed, etc.

protected:
	std::unordered_map<MaterialType, int> productionRates; // How much of each resource the structure produces per internal time unit
	std::unordered_map<MaterialType, int> consumeRates;

public:
	virtual ~Structure() = default;

	virtual void init(int startX, int startY, const std::string& texId);
	virtual void update();
	virtual std::string getTextureId() const;

	int getX() const;
	int getY() const;

	const Inventory& getInternalInventory() const;
};

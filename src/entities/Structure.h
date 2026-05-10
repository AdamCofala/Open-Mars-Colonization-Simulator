#pragma once
#include <string>
#include "Inventory.h"

// TODO : structures may have resources that they produce, we need to include this!

class Structure {
private:
	int x;
	int y;
	std::string textureId;
		// TODO : add more members here, such as production data, resources etc.

public:
	virtual void init(int startX, int startY, const std::string& texId);
	virtual void update(Inventory& globalInventory);
	virtual std::string getTextureId() const;

	virtual int getX() const;
	virtual int getY() const;
};

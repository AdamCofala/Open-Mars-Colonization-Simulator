#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "Structure.h"

class Map {
private:
    int width;
    int height;

    std::vector<Tile> tiles;
    std::vector<Structure> structures;

public:
    void init(int w, int h);
    void generateTerrain();

    Tile& getTile(int x, int y);
	Structure& getStructure(int x, int y);

    int getWidth() const;
    int getHeight() const;
};
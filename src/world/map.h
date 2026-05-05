#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "entities/Structure.h"

// TODO : Please implement methods, getters, setters, and any additional members you think are necessary for the Map class

class Map {
private:
    unsigned int width;
    unsigned int height;

    std::vector<Tile> tiles;
    std::vector<Structure> structures;

public:
    void init(int w = 128, int h = 128);
    void generateTerrain(); // This one u can leave on me

    Tile& getTile(int x, int y) const;
	Structure& getStructure(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    void setStructure(const Structure& newStructure);
};
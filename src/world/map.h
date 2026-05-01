#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "Structure.h"

// TODO : Please implement methods, getters, setters, and any additional members you think are necessary for the Map class

class Map {
private:
    const int width;
    const int height;

    std::vector<Tile> tiles;
    std::vector<Structure> structures;

public:
    void init(int w, int h);
    void generateTerrain(); // This one u can leave on me

    Tile& getTile(int x, int y);
	Structure& getStructure(int x, int y);

    int getWidth() const;
    int getHeight() const;
};
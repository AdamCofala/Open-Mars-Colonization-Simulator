#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "entities/Structure.h"

class Map {
private:
    unsigned int width;
    unsigned int height;

    std::vector<Tile> tiles;
    std::vector<Structure> structures;

public:
    void init(int w, int h);
    void generateTerrain();
    Tile& getTile(int x, int y) const;
    std::vector<Structure>& getStructures();

    int getWidth() const;
    int getHeight() const;

};
#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "entities/Structure.h"
#include "../entities/Pipe.h"
#include "../entities/PipeNetwork.h"

class Map {
private:
    unsigned int width;
    unsigned int height;

    std::vector<Tile> tiles;

    std::vector<std::unique_ptr<Structure>> structures;
    std::vector<std::unique_ptr<PipeNetwork>> allNetworks;

public:
    void init(int w, int h);
    void generateTerrain();
    Tile& getTile(int x, int y) const;

    std::vector<std::unique_ptr<Structure>>& getStructures();
    const std::vector<std::unique_ptr<Structure>>& getStructures() const;

    int getWidth() const;
    int getHeight() const;
    int getHalfWidth() const;
    int getHalfHeight() const;

    bool canPlaceStructure(int x, int y, int xOffset, int yOffset) const;

    void addStructure(std::unique_ptr<Structure> structure);
    void updateNetworks(float dt);
};
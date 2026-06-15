#pragma once
#include <vector>
#include <memory>
#include "Tile.h"
#include "entities/Structure.h"
#include "../entities/Pipe.h"
#include "../entities/PipeNetwork.h"
#include "WorldGenSettings.h"   // NEW

class Map {
private:
    unsigned int width;
    unsigned int height;

    std::vector<Tile> tiles;

    std::vector<std::unique_ptr<Structure>> structures;
    std::vector<std::unique_ptr<PipeNetwork>> allNetworks;

public:
    void init(int w, int h);
    void generateTerrain(const WorldGenSettings& settings = WorldGenSettings{});   // NEW signature

    Tile& getTile(int x, int y) const;

    std::vector<std::unique_ptr<Structure>>& getStructures();
    const std::vector<std::unique_ptr<Structure>>& getStructures() const;

    int getWidth() const;
    int getHeight() const;
    int getHalfWidth() const;
    int getHalfHeight() const;

    bool canPlaceStructure(int x, int y, int xOffset, int yOffset, bool isPipe = false) const;

    void addStructure(std::unique_ptr<Structure> structure);
    void rebuildNetworks();
    void updateNetworks(float dt);
    int  computePipeConnectionMask(int px, int py) const;
    int  computePipeConnectionMaskWithVirtual(int px, int py, int virtualX, int virtualY) const;

    void removeStructureAt(int tileX, int tileY);
};
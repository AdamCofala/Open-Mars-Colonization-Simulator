#pragma once
#include <string>
#include <vector>

// deklaracaja poprzedzaj¹ca bo siê robi³ niez³y Meksyk xD
class Structure;

enum class TileType {
    Normal,
    Ice
};

class Tile {
private:
    int x = 0;
    int y = 0;
    Structure* structure = nullptr;
    bool occupied;
    std::string baseTextureId;
	int slope_data[4]; // [0] north, [1] east, [2] south, [3] west, for example 0000 means flat, 1000 means slope north, 1100 means slope north-east, etc.
	int level = 0;     // Elevation level of the tile (sea level equivalent) - used for rendering order and slope calculations
    TileType type = TileType::Normal;

public:
    void init(int startX, int startY, int slope_data[4]); //Default slop data should be 0000

    bool isOccupied() const;
    bool isFlat() const;
    void setOccupied(bool state);

    TileType getType() const;
    void setType(TileType type);

    int getX() const;
    int getY() const;
    std::string getTextureId() const;
    std::vector<int> getSlopeData() const;
    int getLevel() const;

    void setLevel(int newLevel);
    void setTextureId(const std::string& textureId);
    void setSlopeData(int newSlopeData[4]);

    void setStructure(Structure* newStructure);
    Structure* getStructure() const;
};
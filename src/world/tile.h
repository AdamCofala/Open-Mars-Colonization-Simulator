#pragma once
#include <string>

// TODO : Please implement methods, getters, setters, and any additional members you think are necessary for the Tile class

class Tile {
private:
    int x;
    int y;
    bool occupied;
    std::string baseTextureId;
    int slope_data[4]; // [0] north, [1] east, [2] south, [3] west, for example 0000 means flat, 1000 means slope north, 1100 means slope north-east, etc.
	int level = 0; // Level of the tile (poziom morza type shit), used for rendering order and slope calculations

public:
    void init(int startX, int startY, int slope_data[4]); //Default slop data should be 0000

    bool isOccupied() const;
    void setOccupied(bool state);

    int getX() const;
    int getY() const;
    std::string getTextureId() const;
    int getSlopeData() const;
};
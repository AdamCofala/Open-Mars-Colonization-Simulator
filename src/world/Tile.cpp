#include "Tile.h"

void Tile::init(int startX, int startY, int init_slope_data[4]) {
    x = startX;
    y = startY;
    occupied = false;
    baseTextureId = "";
    level = 0;

    if (init_slope_data != nullptr) {
        for (int i = 0; i < 4; ++i) {
            slope_data[i] = init_slope_data[i];
        }
    }
    else {
        for (int i = 0; i < 4; ++i) {
            slope_data[i] = 0;
        }
    }
}

bool Tile::isOccupied() const {
    return occupied;
}

bool Tile::isFlat() const
{
    return std::max({slope_data[0], slope_data[1], slope_data[2], slope_data[3]}) == 0;
}

void Tile::setOccupied(bool state) {
    occupied = state;
}

int Tile::getX() const {
    return x;
}

int Tile::getY() const {
    return y;
}

std::string Tile::getTextureId() const {
    return baseTextureId;
}

std::vector<int> Tile::getSlopeData() const {
    std::vector<int> slopeVector(slope_data, slope_data + 4);
    return slopeVector;
}

int Tile::getLevel() const {
    return level;
}

void Tile::setLevel(int newLevel) {
    level = newLevel;
}

void Tile::setTextureId(const std::string& textureId) {
    baseTextureId = textureId;
}

void Tile::setSlopeData(int newSlopeData[4]) {
    for (int i = 0; i < 4; ++i) {
        slope_data[i] = newSlopeData[i];
    }
}

void Tile::setStructure(Structure* newStructure) {
    structure = newStructure;
}

const Structure* Tile::getStructure() const {
    return structure;
}
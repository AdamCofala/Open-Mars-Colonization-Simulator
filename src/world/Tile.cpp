#include "Tile.h"
#include <fstream>

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

TileType Tile::getType() const {
    return type;
}

void Tile::setType(TileType newType) {
    type = newType;
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

Structure* Tile::getStructure() const {
    return structure;
}

void Tile::setStructure(Structure* newStructure) {
    structure = newStructure;
	setOccupied(newStructure != nullptr);
}

void Tile::save(std::ofstream& file) const {
    file.write((char*)&x, sizeof(x));
    file.write((char*)&y, sizeof(y));

    file.write((char*)&occupied, sizeof(occupied));

    file.write((char*)&level, sizeof(level));
    file.write((char*)&type, sizeof(type));

    // slope_data
    file.write((char*)slope_data, sizeof(slope_data));

    // textureId
    size_t len = baseTextureId.size();
    file.write((char*)&len, sizeof(len));
    file.write(baseTextureId.c_str(), len);
}

void Tile::load(std::ifstream& file) {
    file.read((char*)&x, sizeof(x));
    file.read((char*)&y, sizeof(y));

    file.read((char*)&occupied, sizeof(occupied));

    file.read((char*)&level, sizeof(level));
    file.read((char*)&type, sizeof(type));

    file.read((char*)slope_data, sizeof(slope_data));

    size_t len;
    file.read((char*)&len, sizeof(len));

    baseTextureId.resize(len);
    file.read(&baseTextureId[0], len);

    structure = nullptr;
}
#pragma once
#include <string>

class Tile {
private:
    int x;
    int y;
    bool occupied;
    std::string baseTextureId;

public:
    void init(int startX, int startY, const std::string& textureId);

    bool isOccupied() const;
    void setOccupied(bool state);

    int getX() const;
    int getY() const;
    std::string getTextureId() const;
};
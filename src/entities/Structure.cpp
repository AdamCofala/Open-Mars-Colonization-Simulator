#include "Structure.h"

void Structure::init(int startX, int startY, const std::string& texId) {
    x = startX;
    y = startY;
    textureId = texId;
}

void Structure::update(Inventory& globalInventory) {

}

std::string Structure::getTextureId() const {
    return textureId;
}

int Structure::getX() const {
    return x;
}

int Structure::getY() const {
    return y;
}
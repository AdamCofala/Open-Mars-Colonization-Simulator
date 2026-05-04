void Tile::init(int startX, int startY, int* init_slope_data) {
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
    // kiedy nie nic nie mówisz to masz p³aski teren jak decha
    else {
        for (int i = 0; i < 4; ++i) {
            slope_data[i] = 0;
        }
    }
}

bool Tile::isOccupied() const {
    return occupied;
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

const int Tile::getSlopeData() const {
    return slope_data;
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

void Tile::setSlopeData(const int newSlopeData[4]) {
    for (int i = 0; i < 4; ++i) {
        slope_data[i] = newSlopeData[i];
    }
}
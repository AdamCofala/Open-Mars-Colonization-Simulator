#pragma once

#include "raylib.h"

class Renderer;
class Map;

class InputManager
{
private:

    Vector2 mouseScreen = { 0.0f, 0.0f };
    Vector2 mouseWorld = { 0.0f, 0.0f };

    int hoveredTileX = -1;
    int hoveredTileY = -1;

public:

    void update(Renderer& renderer, Map& map);

    bool isLeftClicked() const;

    bool isTileHovered() const;

    int getHoveredTileX() const;
    int getHoveredTileY() const;

    Vector2 getMouseScreen() const;
    Vector2 getMouseWorld() const;
};
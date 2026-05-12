#include "InputManager.h"

#include "rendering/Renderer.h"
#include "world/Map.h"
#include "raylib.h"

#include <cmath>

void InputManager::update(Renderer& renderer, Map& map)
{
    mouseScreen = GetMousePosition();

    mouseWorld = GetScreenToWorld2D(mouseScreen, renderer.getGameCamera().getCamera());

    Vector2 iso = renderer.ScreenToIso(mouseWorld);

    hoveredTileX = static_cast<int>(floor(iso.x));
    hoveredTileY = static_cast<int>(floor(iso.y));

    if (hoveredTileX < 0 ||hoveredTileY < 0 || hoveredTileX >= map.getWidth() || hoveredTileY >= map.getHeight())
    {
        hoveredTileX = -1;
        hoveredTileY = -1;
    }

    renderer.RenderHoveredTile(hoveredTileX, hoveredTileY);
}


bool InputManager::isLeftClicked() const
{
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool InputManager::isTileHovered() const
{
    return hoveredTileX != -1;
}

int InputManager::getHoveredTileX() const
{
    return hoveredTileX;
}

int InputManager::getHoveredTileY() const
{
    return hoveredTileY;
}

Vector2 InputManager::getMouseScreen() const
{
    return mouseScreen;
}

Vector2 InputManager::getMouseWorld() const
{
    return mouseWorld;
}
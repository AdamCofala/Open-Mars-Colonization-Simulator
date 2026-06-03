#pragma once
#pragma once
#include "raylib.h"
#include "world/Map.h"

class Renderer;
class Gui;

class InputManager
{
public:
    void init(Map* map, Renderer* renderer, Gui* gui);
    void update();
    Vector2 getSelectedTile() const;   // Returns tile coordinates (col, row) or {-1, -1}
    void updateTileSelection();        // Updates m_selectedTile based on mouse position

private:
    Map* m_map = nullptr;
    Renderer* m_renderer = nullptr;
    Gui* m_gui = nullptr;
    Vector2 m_selectedTile = { -1, -1 };
    Vector2 m_selectedTileOffset = { 3, 3 };

    bool IsPointInConvexPolygon(Vector2 point, const std::vector<Vector2>& poly) const;
    bool m_selected_valid();
};

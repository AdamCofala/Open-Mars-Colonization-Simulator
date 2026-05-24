#pragma once
#pragma once
#include "raylib.h"
#include "world/Map.h"   // zak³adam, ¿e Map jest dostêpna

class Renderer; // forward
class Gui;

class InputManager
{
public:
    void init(Map* map, Renderer* renderer, Gui* gui);
    void update();
    Vector2 getSelectedTile() const;   // zwraca wspó³rzêdne kafelka (col, row) lub {-1,-1}
    void updateTileSelection();        // aktualizuje m_selectedTile na podstawie pozycji myszy i 

private:
    Map* m_map = nullptr;
    Renderer* m_renderer = nullptr;
    Gui* m_gui = nullptr;
    Vector2 m_selectedTile = { -1, -1 };
    Vector2 m_selectedTileOffset = { 3, 3 };

    bool IsPointInConvexPolygon(Vector2 point, const std::vector<Vector2>& poly) const;
    bool m_selected_valid();
};

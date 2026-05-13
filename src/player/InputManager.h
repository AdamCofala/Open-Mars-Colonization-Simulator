#pragma once
#include "raylib.h"
#include "world/Map.h"   // zak³adam, ¿e Map jest dostêpna

class Renderer; // forward

class InputManager
{
public:
    void init(const Map* map, const Renderer* renderer);
    void update();
    Vector2 getSelectedTile() const;   // zwraca wspó³rzêdne kafelka (col, row) lub {-1,-1}

private:
    const Map* m_map = nullptr;
    const Renderer* m_renderer = nullptr;
    Vector2 m_selectedTile = { -1, -1 };

    bool IsPointInConvexPolygon(Vector2 point, const std::vector<Vector2>& poly) const;
};
#include "InputManager.h"
#include "rendering/Renderer.h"
#include <algorithm>

void InputManager::init(const Map* map, const Renderer* renderer)
{
    m_map = map;
    m_renderer = renderer;
}

void InputManager::update()
{
    if (!m_map || !m_renderer) return;

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), m_renderer->getGameCamera().getCamera());
    m_selectedTile = { -1, -1 };

    int width = m_map->getWidth();
    int height = m_map->getHeight();
    int halfW = width / 2;
    int halfH = height / 2;

    // 1. Oblicz granice izometryczne widoczne na ekranie (identycznie jak w Renderer::RenderTerrain)
    Camera2D cam = m_renderer->getGameCamera().getCamera();
    Vector2 corners[4] = {
        GetScreenToWorld2D({0, 0}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0}, cam),
        GetScreenToWorld2D({0, (float)GetScreenHeight()}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, cam)
    };

    int minIsoX = INT_MAX, maxIsoX = INT_MIN, minIsoY = INT_MAX, maxIsoY = INT_MIN;
    for (auto& c : corners) {
        Vector2 iso = m_renderer->ScreenToIso(c);
        minIsoX = std::min(minIsoX, (int)floor(iso.x));
        maxIsoX = std::max(maxIsoX, (int)ceil(iso.x));
        minIsoY = std::min(minIsoY, (int)floor(iso.y));
        maxIsoY = std::max(maxIsoY, (int)ceil(iso.y));
    }

    // Dodaj margines (jak w RenderTerrain: -2,+2)
    minIsoX -= 2; maxIsoX += 2;
    minIsoY -= 2; maxIsoY += 2;

    // Ogranicz do obszaru mapy w przestrzeni izometrycznej
    minIsoX = std::max(minIsoX, -halfW);
    maxIsoX = std::min(maxIsoX, halfW);
    minIsoY = std::max(minIsoY, -halfH);
    maxIsoY = std::min(maxIsoY, halfH);

    // 2. Przekształć granice izometryczne na granice siatki (grid)
    int gridMinX = minIsoX + halfW;
    int gridMaxX = maxIsoX + halfW;
    int gridMinY = minIsoY + halfH;
    int gridMaxY = maxIsoY + halfH;

    // 3. Iteruj tylko po widocznych kafelkach, w odwrotnej kolejności rysowania
    //    Kolejność rysowania: rosnący Y, potem rosnący X → odwrotna = malejący Y, malejący X
    for (int y = gridMaxY; y >= gridMinY; --y) {
        for (int x = gridMaxX; x >= gridMinX; --x) {
            if (x < 0 || x >= width || y < 0 || y >= height) continue;

            const Tile& tile = m_map->getTile(x, y);
            auto slope = tile.getSlopeData();

            // Współrzędne izometryczne dla narożników
            Vector2 baseN = m_renderer->IsoToScreen(x - halfW, y - halfH);
            Vector2 baseE = m_renderer->IsoToScreen(x + 1 - halfW, y - halfH);
            Vector2 baseS = m_renderer->IsoToScreen(x + 1 - halfW, y + 1 - halfH);
            Vector2 baseW = m_renderer->IsoToScreen(x - halfW, y + 1 - halfH);

            int level = tile.getLevel();
            int hN = level + slope[0];
            int hE = level + slope[1];
            int hS = level + slope[2];
            int hW = level + slope[3];
            const int HO = m_renderer->getHeightOffset();

            std::vector<Vector2> corners = {
                {baseN.x, baseN.y - hN * HO},
                {baseE.x, baseE.y - hE * HO},
                {baseS.x, baseS.y - hS * HO},
                {baseW.x, baseW.y - hW * HO}
            };

            if (IsPointInConvexPolygon(mouseWorld, corners)) {
                m_selectedTile = { (float)x, (float)y };
                return;
            }
        }
    }
}

Vector2 InputManager::getSelectedTile() const
{
    return m_selectedTile;
}

bool InputManager::IsPointInConvexPolygon(Vector2 point, const std::vector<Vector2>& poly) const
{
    if (poly.size() < 3) return false;
    // Sprawdzanie znaku iloczynów wektorowych
    int sign = 0;
    for (size_t i = 0; i < poly.size(); i++)
    {
        Vector2 a = poly[i];
        Vector2 b = poly[(i + 1) % poly.size()];
        float cross = (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x);
        if (cross != 0.0f)
        {
            int newSign = (cross > 0) ? 1 : -1;
            if (sign == 0) sign = newSign;
            else if (sign != newSign) return false;
        }
    }
    return true;
}
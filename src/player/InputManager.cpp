#include "InputManager.h"
#include "rendering/Renderer.h"
#include "structures/SolarPanel.h"
#include "structures/IceMelter.h"
#include "entities/Pipe.h"
#include "player/Gui.h"
#include "imgui.h"
#include <algorithm>

void InputManager::init(Map* map, Renderer* renderer, Gui* gui)
{
    m_map = map;
    m_renderer = renderer;
    m_gui = gui;
}

void InputManager::update()
{
    Gui::SelectedTool tool = m_gui ? m_gui->getSelectedTool() : Gui::SelectedTool::Select;
    int selectedBuilding = m_gui ? m_gui->getSelectedBuilding() : -1;

    if (tool == Gui::SelectedTool::Select) {
        m_selectedTile = { -1, -1 };
        m_selectedTileOffset = { 1, 1 };
        m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
    }
    else {
        updateTileSelection();
    }

    if (tool == Gui::SelectedTool::Build && selectedBuilding == 0) {
        SolarPanel panel(0, 0);
        m_selectedTileOffset = { (float)panel.getXOffset(), (float)panel.getYOffset() };
        m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
    }
    else if (tool == Gui::SelectedTool::Build && selectedBuilding == 1) {
        Pipe pipe(0, 0);
        m_selectedTileOffset = { (float)pipe.getXOffset(), (float)pipe.getYOffset() };
        m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
    }
    else if (tool == Gui::SelectedTool::Build && selectedBuilding == 2) {
        IceMelter melter(0, 0);
        m_selectedTileOffset = { (float)melter.getXOffset(), (float)melter.getYOffset() };
        m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
    }
    else if (tool != Gui::SelectedTool::Select) {
        m_selectedTileOffset = { 1, 1 };
        m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
    }

    m_renderer->setSelectedBuildingType(selectedBuilding);

    bool canInteractWithGame = !ImGui::GetIO().WantCaptureMouse;

    if (canInteractWithGame && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m_selected_valid()) {
        if (tool == Gui::SelectedTool::Build && selectedBuilding == 0) {
            // Solar panel
            SolarPanel tempPanel(0, 0);
            int sx = (int)m_selectedTile.x;
            int sy = (int)m_selectedTile.y;
            int xOff = tempPanel.getXOffset();
            int yOff = tempPanel.getYOffset();

            if (m_map->canPlaceStructure(sx, sy, xOff, yOff, false)) { // false = nie rura
                auto newBuilding = std::make_unique<SolarPanel>(sx, sy);
                m_map->addStructure(std::move(newBuilding));

                if (m_gui) {
                    m_gui->setSelectedTool(Gui::SelectedTool::Select);
                }
                m_selectedTile = { -1, -1 };
                m_selectedTileOffset = { 1, 1 };
                m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
            }
        }
        else if (tool == Gui::SelectedTool::Build && selectedBuilding == 1) {
            // Pipe
            Pipe tempPipe(0, 0);
            int sx = (int)m_selectedTile.x;
            int sy = (int)m_selectedTile.y;
            int xOff = tempPipe.getXOffset();
            int yOff = tempPipe.getYOffset();

            if (m_map->canPlaceStructure(sx, sy, xOff, yOff, true)) { // true = rura
                auto newPipe = std::make_unique<Pipe>(sx, sy);
                m_map->addStructure(std::move(newPipe));

                // Nie zmieniamy narzędzia, aby móc stawiać wiele rur
                m_selectedTile = { -1, -1 };
                m_selectedTileOffset = { 1, 1 };
                m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
            }
        }
        else if (tool == Gui::SelectedTool::Build && selectedBuilding == 2) {
            IceMelter tempMelter(0, 0);
            int sx = (int)m_selectedTile.x;
            int sy = (int)m_selectedTile.y;
            int xOff = tempMelter.getXOffset();
            int yOff = tempMelter.getYOffset();

            if (m_map->canPlaceStructure(sx, sy, xOff, yOff, false)) {
                auto newMelter = std::make_unique<IceMelter>(sx, sy);
                m_map->addStructure(std::move(newMelter));

                if (m_gui) {
                    m_gui->setSelectedTool(Gui::SelectedTool::Select);
                }
                m_selectedTile = { -1, -1 };
                m_selectedTileOffset = { 1, 1 };
                m_renderer->setSelectedTile(m_selectedTile, m_selectedTileOffset);
            }
        }
    }
}

bool InputManager::m_selected_valid() {
    return (m_selectedTile.x != -1 && m_selectedTile.y != -1);
}

void InputManager::updateTileSelection() {
    if (!m_map || !m_renderer) return;

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), m_renderer->getGameCamera().getCamera());
    m_selectedTile = { -1, -1 };

    int width = m_map->getWidth();
    int height = m_map->getHeight();

    auto [gridMinX, gridMaxX, gridMinY, gridMaxY] = m_renderer->getVisibleTileBounds(*m_map);

    for (int y = gridMaxY; y >= gridMinY; --y) {
        for (int x = gridMaxX; x >= gridMinX; --x) {
            if (x < 0 || x >= width || y < 0 || y >= height) continue;

            const Tile& tile = m_map->getTile(x, y);
            auto slope = tile.getSlopeData();

            Vector2 baseN = m_renderer->IsoToScreen(x, y, m_map);
            Vector2 baseE = m_renderer->IsoToScreen(x + 1, y, m_map);
            Vector2 baseS = m_renderer->IsoToScreen(x + 1, y + 1, m_map);
            Vector2 baseW = m_renderer->IsoToScreen(x, y + 1, m_map);

            int level = tile.getLevel();
            int hN = level + slope[0];
            int hE = level + slope[1];
            int hS = level + slope[2];
            int hW = level + slope[3];
            const int HO = m_renderer->getHeightOffset();

            std::vector<Vector2> corners = {
                { baseN.x, baseN.y - hN * HO },
                { baseE.x, baseE.y - hE * HO },
                { baseS.x, baseS.y - hS * HO },
                { baseW.x, baseW.y - hW * HO }
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
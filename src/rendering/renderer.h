#pragma once
#include "raylib.h"
#include "GameCamera.h"
#include "TextureManager.h"
#include "entities/Structure.h"
#include "world/World.h"
#include "structures/SolarPanel.h"
#include "structures/IceMelter.h"
#include "entities/Pipe.h"
#include "player/Gui.h"
#include <memory>

struct VisibleTileBounds {
    int minX, maxX;
    int minY, maxY;
};

class Renderer {
public:
    void init();
    void update(float dt);
    void draw(const World& world);
    void shutdown();

    GameCamera& getGameCamera() const;
    Vector2 IsoToScreen(float x, float y, const Map* map = nullptr) const;
    Vector2 ScreenToIso(Vector2 pos, const Map* map = nullptr) const;

    void drawCursor() const;

    int getTileSize() const { return tileSize; }
    int getHeightOffset() const { return HEIGHT_OFFSET; }
    const TextureManager& getTextureManager() const { return *m_txtManager; }
    void setSelectedTile(Vector2 tile, Vector2 offset = { 1,1 });
    void setSelectedBuildingType(int typeIndex);
    void setSelectedTool(Gui::SelectedTool tool) { m_selectedTool = tool; }
    VisibleTileBounds getVisibleTileBounds(const Map& map) const;

private:
    const int tileSize = 64;
    const int HEIGHT_OFFSET = 8;
    Vector2 m_selectedTile = { -1, -1 };
    Vector2 m_selectedTileOffset = { 1, 1 };
    int m_selectedBuildingType = -1;

    std::unique_ptr<GameCamera> m_camera;
    std::unique_ptr<TextureManager> m_txtManager;
    Gui::SelectedTool m_selectedTool = Gui::SelectedTool::Select;

    void RenderIsoTile(const Tile& tile, Vector2 pos, Color tint = WHITE) const;
    void RenderTerrain(const Map& map);
    void RenderStructures(const Map& map);
    void RenderStruct(const Structure& structure, Vector2 pos, const Tile& baseTile, Color tint = WHITE);
    void RenderPipe(int mask, Vector2 pos, const Tile& baseTile, Color tint);
    void RenderSelected(const Map& map, Vector2 offset = { 1,1 }, Color tint = Fade(SKYBLUE, 0.5f));
    int getPipeConnectionBit(int fromX, int fromY, int toX, int toY) const;
};
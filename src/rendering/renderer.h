#pragma once
#include "raylib.h"
#include "GameCamera.h"
#include "TextureManager.h"
#include "world/World.h"

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
    Vector2 IsoToScreen(int x, int y) const;
    Vector2 ScreenToIso(Vector2 pos) const;

    int getTileSize() const { return tileSize; }
    int getHeightOffset() const { return HEIGHT_OFFSET; }
    void setSelectedTile(Vector2 tile) { m_selectedTile = tile; }

    VisibleTileBounds getVisibleTileBounds(int mapWidth, int mapHeight) const;

private:
    const int tileSize = 64;
    const int HEIGHT_OFFSET = 8;
    Vector2 m_selectedTile = { -1, -1 };
    GameCamera* camera = nullptr;
    TextureManager* txt_manager = nullptr;

    void DrawIsoTile(const Tile& tile, Vector2 pos, Color tint = WHITE) const;
    void RenderTerrain(const Map& map);
    void RenderSelected(const Map& map);
};
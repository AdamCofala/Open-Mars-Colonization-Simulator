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
    Vector2 IsoToScreen(float x, float y, const Map* map = nullptr) const;
    Vector2 ScreenToIso(Vector2 pos, const Map* map = nullptr) const;

    void drawCursor() const;

    int getTileSize() const { return tileSize; }
    int getHeightOffset() const { return HEIGHT_OFFSET; }
    void setSelectedTile(Vector2 tile, Vector2 offset = { 1,1 });
    VisibleTileBounds getVisibleTileBounds(const Map& map) const;

private:
    const int tileSize = 64;
    const int HEIGHT_OFFSET = 8;
    Vector2 r_selectedTile = { -1, -1 };
	Vector2 r_selectedTileOffset = { 1, 1 };

    GameCamera* camera = nullptr;
    TextureManager* txt_manager = nullptr;

    void DrawIsoTile(const Tile& tile, Vector2 pos, Color tint = WHITE) const;
    void RenderTerrain(const Map& map);
    void RenderStructures(const Map& map);
    void RenderSelected(const Map& map, Vector2 offset={1,1}, Color tint = Fade(SKYBLUE, 0.5f));
};
#pragma once
#include "raylib.h"
#include "GameCamera.h"
#include "TextureManager.h"
#include "world/World.h"

class Renderer {
public:
    void init();
    void update(float dt);
    void draw(const World& world);
    void shutdown();

private:
    const int tileSize = 64;
	const int HEIGHT_OFFSET = 8;

    GameCamera* camera = nullptr;
	TextureManager* txt_manager = nullptr;

    Vector2 IsoToScreen(int x, int y) const;
    Vector2 ScreenToIso(Vector2 pos) const;
    void DrawIsoTile(const Tile& tile, Vector2 pos) const;
	void RenderTerrain(const Map& map);
};
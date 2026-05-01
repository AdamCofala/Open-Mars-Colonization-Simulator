#pragma once
#include "raylib.h"
#include "GameCamera.h"

class Renderer {
public:
    void init();
    void update(float dt);
    void draw();
    void shutdown();

	// Temporary here, will be moved to a tile structure later
    Texture2D tileTexture = LoadTexture(RESOURCES_PATH "Tile.png");

private:
    const int tileSize = 64;
	const int gridWidth = 128;
	const int gridHeight = 128;

    GameCamera camera = GameCamera(IsoToScreen(0,0));
    Vector2 IsoToScreen(int x, int y) const;
    Vector2 ScreenToIso(Vector2 pos) const;
    void DrawIsoTile(Vector2 p, int size) const;
    void DrawIsoGrid();
};
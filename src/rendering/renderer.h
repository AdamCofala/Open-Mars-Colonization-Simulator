#pragma once
#include "raylib.h"
#include "GameCamera.h"
#include "TextureManager.h"
#include "world/Map.h"

class Renderer {
public:
    Renderer() = default;
    void init();
    void update(float dt);
    void draw(const Map& map);
    void shutdown();

    ~Renderer() = default;

private:

    const int tileSize = 64;
	const int HEIGHT_OFFSET = 8;

    GameCamera camera = GameCamera(IsoToScreen(0, 0));
	TextureManager txt_manager;
    Vector2 IsoToScreen(int x, int y) const;
    Vector2 ScreenToIso(Vector2 pos) const;
    void DrawIsoTile(const Tile& tile, Vector2 pos) const;
};
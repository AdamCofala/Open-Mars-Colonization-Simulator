#pragma once
#include "raylib.h"
#include "GameCamera.h"

class Renderer {
public:
    void init();
    void update(float dt);
    void draw();
    void shutdown();

private:
    const int tileSize = 64;
    GameCamera camera = GameCamera(IsoToScreen(0,0));
    Vector2 IsoToScreen(int x, int y);
    void DrawIsoGrid(int w, int h);
};
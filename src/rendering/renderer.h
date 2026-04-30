#pragma once
#include "raylib.h"

class Renderer {
public:
    void init();
    void update(float dt);
    void draw();
    void shutdown();

private:
    Camera2D camera{};

    float moveSpeed = 500.0f;
    float zoomSpeed = 0.1f;

    const int tileSize = 64;

private:
    Vector2 IsoToScreen(int x, int y);
    void DrawIsoGrid(int w, int h);
};
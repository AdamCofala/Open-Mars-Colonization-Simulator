#include "Renderer.h"

void Renderer::init() {
   
}

void Renderer::update(float dt) {
	camera.update(dt);
}

void Renderer::draw() {
    BeginMode2D(camera.getCamera());

    DrawIsoGrid(30, 30);

    // center marker
    Vector2 center = IsoToScreen(0, 0);
    DrawCircle(center.x, center.y, 6, RED);

    EndMode2D();

    DrawText("Mars TTD - Isometric Prototype", 10, 10, 20, WHITE);
    DrawFPS(10, 40);

}

void Renderer::shutdown() {
    // nothing yet
}

Vector2 Renderer::IsoToScreen(int x, int y) {
    Vector2 out;
    out.x = (x - y) * (tileSize / 2.0f);
    out.y = (x + y) * (tileSize / 4.0f);
    return out;
}

void DrawIsoTile(Vector2 p, int size) {
    Vector2 top = { p.x, p.y };
    Vector2 right = { p.x + size / 2, p.y + size / 4 };
    Vector2 bottom = { p.x, p.y + size / 2 };
    Vector2 left = { p.x - size / 2, p.y + size / 4 };

    DrawLineV(top, right, DARKGRAY);
    DrawLineV(right, bottom, DARKGRAY);
    DrawLineV(bottom, left, DARKGRAY);
    DrawLineV(left, top, DARKGRAY);
}

void Renderer::DrawIsoGrid(int w, int h) {
    for (int x = -w; x < w; x++) {
        for (int y = -h; y < h; y++) {

            Vector2 p = IsoToScreen(x, y);
            DrawIsoTile(p, tileSize);
        }

    }
}
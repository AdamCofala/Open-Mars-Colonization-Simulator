#include "Renderer.h"

void Renderer::init() {
   
}

void Renderer::update(float dt) {
	camera.update(dt);
}

void Renderer::draw() {
    BeginMode2D(camera.getCamera());

    DrawIsoGrid(128, 128);

    Vector2 center = IsoToScreen(0, 0);
    DrawCircle(center.x, center.y, 6, RED);

    EndMode2D();

    DrawText("Mars TTD - Isometric Prototype", 10, 10, 20, WHITE);
    DrawFPS(10, 40);

}

void Renderer::shutdown() {
    UnloadTexture(tileTexture);
}

Vector2 Renderer::IsoToScreen(int x, int y) {
    Vector2 out;
    out.x = (x - y) * (tileSize / 2.0f);
    out.y = (x + y) * (tileSize / 4.0f);
    return out;
}

Vector2 Renderer::ScreenToIso(Vector2 pos) {
    float x = pos.x;
    float y = pos.y;

    float isoX = (x / (tileSize / 2.0f) + y / (tileSize / 4.0f)) / 2.0f;
    float isoY = (y / (tileSize / 4.0f) - (x / (tileSize / 2.0f))) / 2.0f;

    return { isoX, isoY };
}

void Renderer::DrawIsoTile(Vector2 p, int size) {

    DrawTexture(tileTexture, p.x, p.y, WHITE);

}

void Renderer::DrawIsoGrid(int w, int h) {
    for (int x = -w; x < w; x++) {
        for (int y = -h; y < h; y++) {

            Vector2 p = IsoToScreen(x, y);
            DrawIsoTile(p, tileSize);
        }

    }
}
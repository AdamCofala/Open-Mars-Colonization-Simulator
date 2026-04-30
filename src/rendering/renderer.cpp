#include "renderer.h"

void Renderer::init() {
    camera.target = { 0.0f, 0.0f };
    camera.offset = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void Renderer::update(float dt) {

    // MOVE CAMERA (WASD)
    if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed * dt;
    if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed * dt;
    if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed * dt;
    if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed * dt;

    // ZOOM
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.zoom += wheel * zoomSpeed;
        if (camera.zoom < 0.2f) camera.zoom = 0.2f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    }

    // DRAG (MMB)
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        camera.target.x -= delta.x / camera.zoom;
        camera.target.y -= delta.y / camera.zoom;
    }
}

void Renderer::draw() {
    BeginMode2D(camera);

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
#include "rendering/Renderer.h"
#include "raylib.h"

int main() {
    InitWindow(1280, 720, "Mars TTD - Isometric Prototype");

    Renderer renderer;
    renderer.init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        renderer.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        renderer.draw();

        EndDrawing();
    }

    renderer.shutdown();
    CloseWindow();

    return 0;
}
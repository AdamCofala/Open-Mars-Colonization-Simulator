#include "rendering/Renderer.h"
#include "raylib.h"

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

Renderer* renderer = nullptr;

void init()
{
	// TODO add throw if renderer is already initialized and delete it before reinitializing, also check width and height
    // Any global initialization can go here
	InitWindow(WIDTH, HEIGHT, "Mars TTD - Isometric Prototype");
	renderer = new Renderer();
	renderer->init();
}

void update(float dt)
{
    // Any global update logic can go here
	renderer->update(dt);
}

void draw()
{
    // Any global drawing logic can go here
    BeginDrawing();
    ClearBackground(BLACK);
    renderer->draw();
    EndDrawing();
}


int main() {
    init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

		update(dt);
        draw();
    }

    renderer->shutdown();
	delete renderer;
    CloseWindow();

    return 0;
}
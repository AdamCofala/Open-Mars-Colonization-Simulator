#include "rendering/Renderer.h"
#include "world/Map.h"
#include "raylib.h"

static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;

static constexpr unsigned int MAP_WIDTH = 128;
static constexpr unsigned int MAP_HEIGHT = 128;

Renderer* renderer = nullptr;
Map* map = nullptr;


void init()
{
	// TODO add throw if renderer is already initialized and delete it before reinitializing, also check width and height
    // Any global initialization can go here
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Mars TTD - Isometric Prototype");
	renderer = new Renderer();
	renderer->init();

    map = new Map();
    map->init(MAP_WIDTH, MAP_HEIGHT);
	map->generateTerrain();
    
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
    renderer->draw(*map);
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
#include "rendering/Renderer.h"
#include "raylib.h"
#include "world/World.h"
#include "player/InputManager.h"

static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;

static constexpr unsigned int MAP_WIDTH = 128;
static constexpr unsigned int MAP_HEIGHT = 128;

Renderer* renderer = nullptr;
World* world = nullptr;
InputManager* input = nullptr;


void init()
{
    // TODO add throw if renderer is already initialized and delete it before reinitializing, also check width and height
    // Any global initialization can go here
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open Mars 1.0.0-alpha.1");
    renderer = new Renderer();
    renderer->init();

    world = new World();
    world->init(MAP_WIDTH, MAP_HEIGHT);

    input = new InputManager();
}

void update(float dt)
{
    // Any global update logic can go here
    world->update(dt);
	renderer->update(dt);
}

void draw()
{
    // Any global drawing logic can go here
    BeginDrawing();
    ClearBackground(BLACK);
    renderer->draw(*world);
    input->update(*renderer, world->getMap());
    EndDrawing();
}

void clean() {
    renderer->shutdown();
	delete renderer;

    world->shutdown();
    delete world;

	delete input;
}


int main() {
    init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
		update(dt);
        draw();
    }

    CloseWindow();
    clean();

    return 0;
}

#include "rendering/Renderer.h"
#include "raylib.h"
#include "world/World.h"
#include "player/InputManager.h"
#include "player/Gui.h"

static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;

static constexpr unsigned int MAP_WIDTH = 128;
static constexpr unsigned int MAP_HEIGHT = 128;

Renderer* renderer = nullptr;
World* world = nullptr;
InputManager* inputManager = nullptr;
Gui* gui = nullptr;


void init()
{
    // Any global initialization can go here
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open Mars 1.0.0-alpha.1");
    renderer = new Renderer();
    renderer->init();

    world = new World();
    world->init(MAP_WIDTH, MAP_HEIGHT);

	gui = new Gui();
    gui->init(world);

    inputManager = new InputManager();
    inputManager->init(&world->getMap(), renderer, gui);

}

void update(float dt) {
    inputManager->update();
    float speed = gui ? gui->getGameSpeed() : 1.0f;
    world->update(dt * speed);
    renderer->update(dt);
}

void draw()
{
    // Any global drawing logic can go here
    BeginDrawing();

    ClearBackground(BLACK);
    renderer->draw(*world);

    Vector2 sel = inputManager->getSelectedTile();
    DrawFPS(10, 30);

    gui->render();

    EndDrawing();
}

void clean() {
    gui->shutdown();
    delete gui;

    renderer->shutdown();
	delete renderer;

    world->shutdown();
    delete world;
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

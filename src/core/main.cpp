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
InputManager* inputManager = nullptr;


void init()
{
    // TODO add throw if renderer is already initialized and delete it before reinitializing, also check width and height
    // Any global initialization can go here
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open Mars 1.0.0-alpha.1");
    renderer = new Renderer();
    renderer->init();

    world = new World();
    world->init(MAP_WIDTH, MAP_HEIGHT);

	inputManager = new InputManager();
    inputManager->init(&world->getMap(), renderer);

}

void update(float dt) {
    inputManager->update();
    world->update(dt);
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
    DrawText(TextFormat("Selected: %d, %d", (int)sel.x, (int)sel.y), 10, 60, 20, YELLOW);
	DrawText(TextFormat("Day: %d, Month: %d, Year: %d", world->getDay(), world->getMonth(), world->getYear()), 10, 90, 20, ORANGE);

    EndDrawing();
}

void clean() {
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

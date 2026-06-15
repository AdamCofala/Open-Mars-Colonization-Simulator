#include "rendering/Renderer.h"
#include "raylib.h"
#include "world/World.h"
#include "player/InputManager.h"
#include "player/Gui.h"
#include "player/StartScreen.h"
#include "imgui.h"
#include "rlImGui.h"

static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;

// ─── Global state ─────────────────────────────────────────────────────────────

enum class GameState {
    StartScreen,
    Playing
};

static GameState    g_state = GameState::StartScreen;
static StartScreen* g_startScreen = nullptr;

static Renderer* renderer = nullptr;
static World* world = nullptr;
static InputManager* inputManager = nullptr;
static Gui* gui = nullptr;

// ─── Game lifecycle ───────────────────────────────────────────────────────────

void initGame(const WorldGenSettings& settings) {
    renderer = new Renderer();
    renderer->init();

    world = new World();
    world->init(settings.mapWidth, settings.mapHeight, settings);

    gui = new Gui();
    gui->init(world, renderer);

    inputManager = new InputManager();
    inputManager->init(&world->getMap(), renderer, gui);
}

void shutdownGame() {
    if (gui) { gui->shutdown();      delete gui;          gui = nullptr; }
    if (renderer) { renderer->shutdown(); delete renderer;     renderer = nullptr; }
    if (world) { world->shutdown();    delete world;        world = nullptr; }
    if (inputManager) { delete inputManager; inputManager = nullptr; }
}

// ─── Init / update / draw ─────────────────────────────────────────────────────

void init() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open Mars 1.0.0-alpha.1");
    HideCursor();

    rlImGuiSetup(true);

    g_startScreen = new StartScreen();
    g_startScreen->init();
}

void update(float dt) {
    if (g_state == GameState::Playing) {
        inputManager->update();
        float speed = gui ? gui->getGameSpeed() : 1.0f;
        world->update(dt * speed);
        renderer->update(dt);
    }
}

void draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    rlImGuiBegin();

    if (g_state == GameState::StartScreen) {
        StartScreen::Action action = g_startScreen->render();

        if (action == StartScreen::Action::CreateWorld) {
            WorldGenSettings settings = g_startScreen->getGenSettings();
            g_startScreen->shutdown();
            delete g_startScreen;
            g_startScreen = nullptr;

            initGame(settings);
            g_state = GameState::Playing;
        }
        else if (action == StartScreen::Action::LoadWorld) {
            std::string path = g_startScreen->getLoadPath();
            // NYI — podepnij tu deserializację
            (void)path;

            WorldGenSettings defaults;
            g_startScreen->shutdown();
            delete g_startScreen;
            g_startScreen = nullptr;

            initGame(defaults);
            g_state = GameState::Playing;
        }
        else if (action == StartScreen::Action::Quit) {
            CloseWindow();
        }
    }
    else if (g_state == GameState::Playing) {
        renderer->draw(*world);

        DrawFPS(10, 30);

        gui->render();

        // --- Obsługa "Exit to Menu" z GUI ---
        if (gui->shouldExitToMenu()) {
            shutdownGame();                      // usuwa gui, renderer, world, inputManager

            g_startScreen = new StartScreen();
            g_startScreen->init();

            g_state = GameState::StartScreen;
        }
    }

    rlImGuiEnd();   // <-- ImGui rysuje swoje okna tutaj

    // Kursor musi być narysowany po ImGui, żeby był zawsze na wierzchu
    if (g_state == GameState::Playing && renderer != nullptr) {
        renderer->drawCursor();
    }

    EndDrawing();
}

void clean() {
    shutdownGame();

    if (g_startScreen) {
        g_startScreen->shutdown();
        delete g_startScreen;
        g_startScreen = nullptr;
    }

    rlImGuiShutdown();
}

// ─── Entry point ──────────────────────────────────────────────────────────────

int main() {
    init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        update(dt);
        draw();
    }

    if (IsWindowReady()) CloseWindow();
    clean();

    return 0;
}
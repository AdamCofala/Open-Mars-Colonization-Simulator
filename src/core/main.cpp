#include "rendering/Renderer.h"
#include "raylib.h"
#include "world/World.h"
#include "player/InputManager.h"
#include "player/Gui.h"
#include "player/StartScreen.h"
#include "imgui.h"
#include "rlImGui.h"
#include <memory>

static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;


enum class GameState {
    StartScreen,
    Playing
};

class Game {
public:
    Game() = default;

    void init() {
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open Mars 1.0.0-alpha.1");
        rlImGuiSetup(true);

        m_startScreen = std::make_unique<StartScreen>();
        m_startScreen->init();
        m_state = GameState::StartScreen;
    }

    void update(float dt) {
        if (m_state == GameState::Playing) {
            m_inputManager->update();
            float speed = m_gui ? m_gui->getGameSpeed() : 1.0f;
            m_world->update(dt * speed);
            m_renderer->update(dt);
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BLACK);

        if (m_state == GameState::StartScreen) {
            ShowCursor();
        }
        else if (m_state == GameState::Playing) {
            HideCursor();
        }

        rlImGuiBegin();

        if (m_state == GameState::StartScreen) {
            StartScreen::Action action = m_startScreen->render();

            if (action == StartScreen::Action::CreateWorld) {
                WorldGenSettings settings = m_startScreen->getGenSettings();
                m_startScreen->shutdown();
                m_startScreen.reset();
                initGame(settings);
                m_state = GameState::Playing;
            }
            else if (action == StartScreen::Action::LoadWorld) {
                WorldGenSettings defaults;
                m_startScreen->shutdown();
                m_startScreen.reset();
                initGame(defaults);
                m_state = GameState::Playing;
            }
            else if (action == StartScreen::Action::Quit) {
                m_shouldQuit = true;
            }
        }
        else if (m_state == GameState::Playing) {
            m_renderer->draw(*m_world);
            DrawFPS(10, 30);
            m_gui->render();

            if (m_gui->shouldExitToMenu()) {
                shutdownGame();
                m_startScreen = std::make_unique<StartScreen>();
                m_startScreen->init();
                m_state = GameState::StartScreen;
            }
        }

        rlImGuiEnd();

        if (m_state == GameState::Playing && m_renderer) {
            m_renderer->drawCursor();
        }

        EndDrawing();
    }

    bool shouldQuit() const { return m_shouldQuit || WindowShouldClose(); }

    void clean() {
        shutdownGame();
        if (m_startScreen) {
            m_startScreen->shutdown();
            m_startScreen.reset();
        }
        rlImGuiShutdown();
        if (IsWindowReady()) CloseWindow();
    }

private:
    GameState m_state = GameState::StartScreen;
    bool m_shouldQuit = false;

    std::unique_ptr<StartScreen> m_startScreen;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<World> m_world;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<Gui> m_gui;

    void initGame(const WorldGenSettings& settings) {
        m_renderer = std::make_unique<Renderer>();
        m_renderer->init();

        m_world = std::make_unique<World>();
        m_world->init(settings.mapWidth, settings.mapHeight, settings);

        m_gui = std::make_unique<Gui>();
        m_gui->init(m_world.get(), m_renderer.get());

        m_inputManager = std::make_unique<InputManager>();
        m_inputManager->init(&m_world->getMap(), m_renderer.get(), m_gui.get());
    }

    void shutdownGame() {
        if (m_gui) { m_gui->shutdown(); m_gui.reset(); }
        m_renderer.reset();
        m_world.reset();
        m_inputManager.reset();
    }
};


int main() {
    Game game;
    game.init();

    while (!game.shouldQuit()) {
        float dt = GetFrameTime();
        game.update(dt);
        game.draw();
    }

    game.clean();
    return 0;
}
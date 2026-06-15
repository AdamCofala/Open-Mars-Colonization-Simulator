#pragma once
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "world/WorldGenSettings.h"
#include <string>
#include <functional>

class StartScreen {
public:
    enum class Action {
        None,
        CreateWorld,
        LoadWorld,
        Quit
    };

    void init();
    void shutdown();

    // Returns the action to take this frame (once per press)
    Action render();

    const WorldGenSettings& getGenSettings() const { return m_settings; }
    const std::string& getLoadPath()    const { return m_loadPath; }

private:
    enum class Screen { Main, Create, Load };

    Screen           m_screen = Screen::Main;
    WorldGenSettings m_settings;
    std::string      m_loadPath;

    // Create-world UI state
    int              m_mapSizeIdx = 1;         // 0=small,1=medium,2=large
    float            m_roughness = 0.5f;      // maps → noiseScale
    float            m_elevation = 0.5f;      // maps → maxHeight
    float            m_lakeFreq = 0.5f;      // maps → lakeThreshold
    int              m_seed = -1;
    bool             m_randomSeed = true;
    char             m_seedBuf[16] = "0";

    // Load-world UI state
    char             m_pathBuf[512] = "";
    bool             m_pathError = false;

    // Background
    float            m_time = 0.0f;

    void renderMain(Action& out);
    void renderCreate(Action& out);
    void renderLoad(Action& out);

    void applySettings();

    // Style helpers
    void pushMarsStyle();
    void popMarsStyle();
    bool bigButton(const char* label, float width = 260.f);
    void centeredText(const char* text, float fontSize = 1.0f);
};
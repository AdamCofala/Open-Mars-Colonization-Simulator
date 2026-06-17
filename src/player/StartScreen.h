#pragma once
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "world/WorldGenSettings.h"
#include <string>
#include <vector>

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
    Action render();
    const WorldGenSettings& getGenSettings() const { return m_settings; }
    const std::string& getLoadPath()    const { return m_loadPath; }

private:
    enum class Screen { Main, Create, Load, Generating };

    Screen           m_screen = Screen::Main;
    WorldGenSettings m_settings;
    std::string      m_loadPath;

    // Create-world UI state
    int              m_mapSizeIdx = 1;
    float            m_roughness = 0.5f;
    float            m_elevation = 0.5f;
    float            m_lakeFreq = 0.5f;
    int              m_seed = -1;
    bool             m_randomSeed = true;
    char             m_seedBuf[16] = "0";

    // Load-world UI state
    std::string      m_filePath;
    bool             m_showFileDialog = false;

    // Generation screen state
    bool             m_genStarted = false;
    float            m_genStartTime = 0.0f;
    std::vector<std::string> m_genMessages;
    int              m_genMessageIdx = 0;

    // Background
    float            m_time = 0.0f;

    void renderMain(Action& out);
    void renderCreate(Action& out);
    void renderLoad(Action& out);
    void renderGenerating(Action& out);

    void applySettings();

    // Style helpers
    void pushGuiStyle();
    void popGuiStyle();
    bool bigButton(const char* label, float width = 260.f);

    // Background drawing (wywoływane w render)
    void drawBackground();
    void drawSun();

    // File dialog pipeline
    static std::string ShowOpenFileDialog();
};
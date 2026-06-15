#include "StartScreen.h"
#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// ─── Colour palette ──────────────────────────────────────────────────────────
static const ImVec4 COL_MARS_DARK = { 0.10f, 0.06f, 0.04f, 1.00f };
static const ImVec4 COL_MARS_MID = { 0.22f, 0.13f, 0.08f, 1.00f };
static const ImVec4 COL_MARS_RUST = { 0.65f, 0.28f, 0.08f, 1.00f };
static const ImVec4 COL_MARS_ORANGE = { 0.90f, 0.46f, 0.14f, 1.00f };
static const ImVec4 COL_MARS_TAN = { 0.82f, 0.68f, 0.52f, 1.00f };
static const ImVec4 COL_TEXT_DIM = { 0.65f, 0.55f, 0.45f, 1.00f };
static const ImVec4 COL_RED_WARN = { 0.85f, 0.20f, 0.10f, 1.00f };

// ─── Init / Shutdown ─────────────────────────────────────────────────────────

void StartScreen::init() {
    m_time = 0.0f;
    srand((unsigned)time(nullptr));
}

void StartScreen::shutdown() {}

// ─── Style ───────────────────────────────────────────────────────────────────

void StartScreen::pushMarsStyle() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_MARS_DARK);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, COL_MARS_MID);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, COL_MARS_RUST);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.10f, 0.06f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.30f, 0.16f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_MARS_RUST);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, COL_MARS_ORANGE);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, COL_MARS_ORANGE);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, COL_MARS_ORANGE);
    ImGui::PushStyleColor(ImGuiCol_Button, COL_MARS_RUST);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_MARS_ORANGE);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.35f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, COL_MARS_RUST);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_MARS_ORANGE);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.75f, 0.35f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Separator, COL_MARS_MID);
    ImGui::PushStyleColor(ImGuiCol_Text, COL_MARS_TAN);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_MARS_RUST);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
}

void StartScreen::popMarsStyle() {
    ImGui::PopStyleVar(6);
    ImGui::PopStyleColor(18);
}

bool StartScreen::bigButton(const char* label, float width) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
    bool pressed = ImGui::Button(label, ImVec2(width, 0));
    ImGui::PopStyleVar();
    return pressed;
}

// ─── Main render dispatch ─────────────────────────────────────────────────────

StartScreen::Action StartScreen::render() {
    m_time += GetFrameTime();

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Animated Mars background
    Color skyTop = { 10, 5, 3, 255 };
    Color skyBot = { 38, 18, 8, 255 };
    DrawRectangleGradientV(0, 0, sw, sh, skyTop, skyBot);

    for (int i = 0; i < 200; i++) {
        float sx = (float)((i * 1619 + 42337) % sw);
        float sy = (float)((i * 7331 + 11317) % (sh / 2));
        float blink = 0.5f + 0.5f * sinf(m_time * 1.3f + i * 0.7f);
        unsigned char bright = (unsigned char)(160 + 95 * blink);
        DrawPixel((int)sx, (int)sy, { bright, bright, (unsigned char)(bright - 20), 255 });
    }

    for (int x = 0; x < sw; x++) {
        float wave = sinf(x * 0.013f) * 18.0f + cosf(x * 0.031f) * 9.0f;
        int   horizon = sh - 90 + (int)wave;
        Color surface = { 120, 52, 18, 255 };
        DrawLine(x, horizon, x, sh, surface);
    }

    Action action = Action::None;

    pushMarsStyle();
    switch (m_screen) {
    case Screen::Main:   renderMain(action);   break;
    case Screen::Create: renderCreate(action); break;
    case Screen::Load:   renderLoad(action);   break;
    }
    popMarsStyle();

    return action;
}

// ─── Main Menu ────────────────────────────────────────────────────────────────

void StartScreen::renderMain(Action& out) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const float winW = 340.0f;
    const float winH = 360.0f;
    ImGui::SetNextWindowPos({ (sw - winW) * 0.5f, (sh - winH) * 0.5f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ winW, winH }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.92f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("##main_menu", nullptr, flags)) {
        float titleW = ImGui::CalcTextSize("OPEN MARS").x * 2.0f;
        ImGui::SetCursorPosX((winW - titleW) * 0.5f);
        ImGui::SetWindowFontScale(2.0f);
        ImGui::TextColored(COL_MARS_ORANGE, "OPEN MARS");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::SetCursorPosX((winW - ImGui::CalcTextSize("Colony Survival Sim").x) * 0.5f);
        ImGui::TextColored(COL_TEXT_DIM, "Colony Survival Sim");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        const float btnW = winW - 48.0f;
        ImGui::SetCursorPosX(24.0f);
        if (bigButton("  > New World", btnW)) {
            m_screen = Screen::Create;
        }

        ImGui::Spacing();
        ImGui::SetCursorPosX(24.0f);
        if (bigButton("  > Load World", btnW)) {
            m_pathError = false;
            m_screen = Screen::Load;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::SetCursorPosX(24.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.08f, 0.04f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.55f, 0.12f, 0.06f, 1.0f));
        if (bigButton("  Quit", btnW)) {
            out = Action::Quit;
        }
        ImGui::PopStyleColor(2);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SetCursorPosX((winW - ImGui::CalcTextSize("v1.0.0-alpha.1").x) * 0.5f);
        ImGui::TextColored(COL_TEXT_DIM, "v1.0.0-alpha.1");
    }
    ImGui::End();
}

// ─── Create World ─────────────────────────────────────────────────────────────

void StartScreen::renderCreate(Action& out) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const float winW = 440.0f;
    const float winH = 480.0f;
    ImGui::SetNextWindowPos({ (sw - winW) * 0.5f, (sh - winH) * 0.5f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ winW, winH }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.93f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("##create_world", nullptr, flags)) {
        ImGui::SetWindowFontScale(1.4f);
        ImGui::TextColored(COL_MARS_ORANGE, "New World");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(COL_MARS_TAN, "Map Size");
        const char* sizes[] = { "Small  (64x64)", "Medium (128x128)", "Large  (192x192)" };
        ImGui::SetNextItemWidth(-1);
        ImGui::Combo("##mapsize", &m_mapSizeIdx, sizes, 3);

        ImGui::Spacing();

        ImGui::TextColored(COL_MARS_TAN, "Terrain");
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Roughness##r", &m_roughness, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Controls terrain noise scale - higher = more chaotic terrain");

        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Elevation##e", &m_elevation, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Controls maximum terrain height");

        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Warp Strength##w", &m_settings.warpStrength, 0.0f, 15.0f, "%.1f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Domain warp distortion - higher = more twisted terrain");

        ImGui::Spacing();

        ImGui::TextColored(COL_MARS_TAN, "Resources");
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Ice Frequency##l", &m_lakeFreq, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("How common ice deposits are in basins");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(COL_MARS_TAN, "Seed");
        ImGui::Checkbox("Random seed", &m_randomSeed);
        if (!m_randomSeed) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::InputText("##seed", m_seedBuf, sizeof(m_seedBuf),
                ImGuiInputTextFlags_CharsDecimal)) {
                m_seed = atoi(m_seedBuf);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const float btnW = (winW - 60.0f) * 0.5f;

        ImGui::SetCursorPosX(24.0f);
        if (bigButton("< Back", btnW)) {
            m_screen = Screen::Main;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(winW - btnW - 24.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.40f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.55f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.30f, 0.08f, 1.0f));
        if (bigButton("Generate >", btnW)) {
            applySettings();
            out = Action::CreateWorld;
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::End();
}

// ─── Load World ───────────────────────────────────────────────────────────────

void StartScreen::renderLoad(Action& out) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const float winW = 440.0f;
    const float winH = 260.0f;
    ImGui::SetNextWindowPos({ (sw - winW) * 0.5f, (sh - winH) * 0.5f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ winW, winH }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.93f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("##load_world", nullptr, flags)) {
        ImGui::SetWindowFontScale(1.4f);
        ImGui::TextColored(COL_MARS_ORANGE, "Load World");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(COL_TEXT_DIM, "Save file path:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##loadpath", m_pathBuf, sizeof(m_pathBuf));

        ImGui::Spacing();
        ImGui::TextColored(COL_TEXT_DIM, "Example: saves/my_colony.omars");

        if (m_pathError) {
            ImGui::Spacing();
            ImGui::TextColored(COL_RED_WARN, "! File not found or invalid path.");
        }

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT_DIM);
        ImGui::TextWrapped("Save/load functionality will be implemented in a future update.");
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const float btnW = (winW - 60.0f) * 0.5f;

        ImGui::SetCursorPosX(24.0f);
        if (bigButton("< Back", btnW)) {
            m_pathError = false;
            m_screen = Screen::Main;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(winW - btnW - 24.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.40f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.55f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.30f, 0.08f, 1.0f));
        if (bigButton("Load >", btnW)) {
            m_loadPath = m_pathBuf;
            if (m_loadPath.empty()) {
                m_pathError = true;
            }
            else {
                m_pathError = false;
                out = Action::LoadWorld;
            }
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::End();
}

// ─── Apply settings ───────────────────────────────────────────────────────────

void StartScreen::applySettings() {
    WorldGenSettings::applyMapSizePreset(m_mapSizeIdx, m_settings);
    m_settings.noiseScale = 0.02f + m_roughness * 0.10f;
    m_settings.maxHeight = 4 + (int)(m_elevation * 12.0f);
    m_settings.lakeThreshold = 0.8f - m_lakeFreq * 0.5f;
    m_settings.seed = m_randomSeed ? (rand() % 100000) : m_seed;
}
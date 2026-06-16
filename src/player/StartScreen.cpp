#include "StartScreen.h"
#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <vector>
#include <cmath>

// ─── Pixel-art star ──────────────────────────────────────────────────────────
struct Star {
    float x, y;
    float size;
    float baseBright;
    float phase;
};

static void DrawPixelBlock(int x, int y, int size, Color color) {
    DrawRectangle(x, y, size, size, color);
}

// ─── Init / Shutdown ─────────────────────────────────────────────────────────
void StartScreen::init() {
    m_time = 0.0f;
    srand((unsigned)time(nullptr));
    m_filePath.clear();
}

void StartScreen::shutdown() {}

// ─── Style – identyczny z Gui::init ──────────────────────────────────────────
void StartScreen::pushGuiStyle() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.86f, 0.92f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.55f, 0.62f, 0.72f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.03f, 0.05f, 0.08f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.03f, 0.05f, 0.08f, 0.88f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.10f, 0.70f, 0.95f, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.02f, 0.05f, 0.09f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.03f, 0.10f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.02f, 0.05f, 0.09f, 0.95f));

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.07f, 0.11f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.10f, 0.18f, 0.26f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.12f, 0.22f, 0.32f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07f, 0.14f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.32f, 0.44f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.12f, 0.40f, 0.55f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.08f, 0.18f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.10f, 0.30f, 0.44f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.12f, 0.38f, 0.55f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.10f, 0.70f, 0.95f, 0.60f));
    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(0.10f, 0.70f, 0.95f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(0.10f, 0.70f, 0.95f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.10f, 0.70f, 0.95f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.10f, 0.70f, 0.95f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.10f, 0.78f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.10f, 0.22f, 0.32f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0.10f, 0.32f, 0.44f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0.12f, 0.40f, 0.55f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.10f, 0.70f, 0.95f, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0.10f, 0.70f, 0.95f, 0.60f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 7.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 9.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 1.0f);
}

void StartScreen::popGuiStyle() {
    ImGui::PopStyleVar(11);
    ImGui::PopStyleColor(29);
}

bool StartScreen::bigButton(const char* label, float width) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
    bool pressed = ImGui::Button(label, ImVec2(width, 0));
    ImGui::PopStyleVar();
    return pressed;
}

std::string StartScreen::ShowOpenFileDialog() {
    // TODO: native file dialog
    return "";
}

// ─── Słońce (wolna parabola, pikselowe) ──────────────────────────────────────
void StartScreen::drawSun() {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    const int horizonBase = sh - 70;
    const float centerX = sw * 0.5f;
    const float sunRadius = 16.0f;           // większe, ładniejsze
    const float sunMaxHeight = 180.0f;
    const float period = 90.0f;              // 90 sekund – bardzo powoli

    float phase = fmodf(m_time, period) / period;
    float x = -sunRadius + (sw + 2.0f * sunRadius) * phase;
    float t = (x - centerX) / (sw * 0.5f);
    t = fmaxf(-1.0f, fminf(1.0f, t));
    float heightFactor = 1.0f - t * t;
    float sunY = horizonBase - sunMaxHeight * heightFactor - sunRadius;

    // Pikselowe słońce (blokowe koło, jak poprzednia planeta)
    int r = (int)sunRadius;
    Color sunColor = { 255, 200, 60, 255 };
    for (int dy = -r; dy <= r; dy += 2) {
        for (int dx = -r; dx <= r; dx += 2) {
            if (dx * dx + dy * dy <= r * r) {
                DrawPixelBlock((int)x + dx, (int)sunY + dy, 2, sunColor);
            }
        }
    }
}

// ─── Tło (gwiazdy, niebo, góry) ──────────────────────────────────────────────
void StartScreen::drawBackground() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Niebo
    Color skyTop = { 18, 10, 20, 255 };
    Color skyBot = { 60, 25, 15, 255 };
    DrawRectangleGradientV(0, 0, sw, sh, skyTop, skyBot);

    // Gwiazdy (jednorazowa inicjalizacja)
    static std::vector<Star> stars;
    if (stars.empty()) {
        std::mt19937 rng((unsigned)time(nullptr));
        std::uniform_real_distribution<float> xDist(0.0f, (float)sw);
        std::uniform_real_distribution<float> yDist(0.0f, (float)sh * 0.6f);
        std::uniform_real_distribution<float> sizeDist(1.0f, 3.0f);
        std::uniform_real_distribution<float> brightDist(0.4f, 1.0f);
        std::uniform_real_distribution<float> phaseDist(0.0f, 2.0f * PI);
        int numStars = 250;
        for (int i = 0; i < numStars; ++i) {
            stars.push_back({ xDist(rng), yDist(rng), sizeDist(rng), brightDist(rng), phaseDist(rng) });
        }
    }

    for (const auto& s : stars) {
        float twinkle = 0.5f + 0.5f * sinf(m_time * 2.5f + s.phase);
        unsigned char alpha = (unsigned char)(255.0f * s.baseBright * twinkle);
        int size = (int)(s.size + 0.5f);
        if (size >= 3) {
            DrawPixelBlock((int)s.x, (int)s.y, 2, Color{ 220, 230, 255, alpha });
        }
        else {
            DrawPixel((int)s.x, (int)s.y, Color{ 200, 210, 240, alpha });
        }
    }

    // Słońce (rysowane przed górami, więc chowa się za nimi)
    drawSun();

    // Pikselowe góry (płaskie centrum, strome brzegi + organiczny szum)
    int horizonBase = sh - 70;
    int blockWidth = 8;
    float maxHillHeight = 80.0f;
    float centerX = sw * 0.5f;

    for (int x = 0; x < sw; x += blockWidth) {
        float dist = fabsf(x - centerX) / (sw * 0.5f);
        float baseHill = maxHillHeight * dist * dist;

        float noiseScale = 12.0f;
        float nx = x * 0.02f;
        float noiseVal = sinf(nx * 1.3f) * 4.0f
            + cosf(nx * 2.7f + 1.0f) * 3.0f
            + sinf(nx * 5.1f + 2.3f) * 2.0f;
        float edgeFactor = dist;
        float combinedHill = baseHill + noiseVal * edgeFactor * noiseScale / 12.0f;

        int horizonY = horizonBase - (int)combinedHill;
        int columnTop = horizonY / blockWidth * blockWidth;
        Color surface = { 120, 52, 18, 255 };
        DrawRectangle(x, columnTop, blockWidth, sh - columnTop, surface);
    }
}

// ─── Główna pętla renderowania ────────────────────────────────────────────────
StartScreen::Action StartScreen::render() {
    m_time += GetFrameTime();
    drawBackground();

    Action action = Action::None;
    pushGuiStyle();

    switch (m_screen) {
    case Screen::Main:        renderMain(action);        break;
    case Screen::Create:      renderCreate(action);      break;
    case Screen::Load:        renderLoad(action);        break;
    case Screen::Generating:  renderGenerating(action);  break;
    }

    popGuiStyle();
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
        ImGui::TextColored(ImVec4(0.10f, 0.70f, 0.95f, 1.0f), "OPEN MARS");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::SetCursorPosX((winW - ImGui::CalcTextSize("Colony Survival Simulator").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.55f, 0.62f, 0.72f, 1.0f), "Colony Survival Simulator");

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
            m_filePath.clear();
            m_screen = Screen::Load;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::SetCursorPosX(24.0f);

        if (bigButton("  Quit", btnW)) {
            out = Action::Quit;
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SetCursorPosX((winW - ImGui::CalcTextSize("v1.0.0-alpha.1").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.55f, 0.62f, 0.72f, 1.0f), "v1.0.0-alpha.1");
    }
    ImGui::End();
}

// ─── Create World (AlwaysAutoResize, bez przewijania) ─────────────────────────
void StartScreen::renderCreate(Action& out) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Okno automatycznie dopasowuje rozmiar – brak sztywnego SetNextWindowSize
    ImGui::SetNextWindowPos({ (float)sw * 0.5f, (float)sh * 0.5f }, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.93f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("##create_world", nullptr, flags)) {
        ImGui::SetWindowFontScale(1.4f);
        ImGui::TextColored(ImVec4(0.10f, 0.70f, 0.95f, 1.0f), "New World");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "Map Size");
        const char* sizes[] = { "Small  (64x64)", "Medium (128x128)", "Large  (192x192)" };
        ImGui::SetNextItemWidth(300.0f);
        ImGui::Combo("##mapsize", &m_mapSizeIdx, sizes, 3);

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "Terrain");
        ImGui::Separator();

        ImGui::Text("Roughness");
        ImGui::SetNextItemWidth(300.0f);
        ImGui::SliderFloat("##roughness", &m_roughness, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Higher = more chaotic terrain");

        ImGui::Text("Elevation");
        ImGui::SetNextItemWidth(300.0f);
        ImGui::SliderFloat("##elevation", &m_elevation, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Higher = taller mountains");

        ImGui::Text("Warp Strength");
        ImGui::SetNextItemWidth(300.0f);
        ImGui::SliderFloat("##warp", &m_settings.warpStrength, 0.0f, 15.0f, "%.1f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Domain warp distortion");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "Resources");
        ImGui::Separator();

        ImGui::Text("Ice Frequency");
        ImGui::SetNextItemWidth(300.0f);
        ImGui::SliderFloat("##ice", &m_lakeFreq, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("How much ice appears");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "Seed");
        ImGui::Checkbox("Random seed", &m_randomSeed);
        if (!m_randomSeed) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::InputText("##seed", m_seedBuf, sizeof(m_seedBuf), ImGuiInputTextFlags_CharsDecimal))
                m_seed = atoi(m_seedBuf);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Dynamiczne przyciski
        float avail = ImGui::GetContentRegionAvail().x;
        float btnW =  100;

        ImGui::SetCursorPosX(60.0f);
        if (bigButton("< Back", btnW)) {
            m_screen = Screen::Main;
        }

        ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.40f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.55f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.30f, 0.08f, 1.0f));
        if (bigButton("Generate >", btnW)) {
            applySettings();
            m_genStarted = false;
            m_genStartTime = m_time;
            m_genMessageIdx = 0;
            m_screen = Screen::Generating;
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::End();
}

// ─── Load World ──────────────────────────────────────────────────────────────
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
        ImGui::TextColored(ImVec4(0.10f, 0.70f, 0.95f, 1.0f), "Load World");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "Select save file:");

        if (ImGui::Button("Browse...", ImVec2(120, 0))) {
            m_filePath = ShowOpenFileDialog();
        }
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.6f);
        ImGui::InputText("##path", const_cast<char*>(m_filePath.c_str()), m_filePath.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered() && !m_filePath.empty()) {
            ImGui::SetTooltip("%s", m_filePath.c_str());
        }

        if (!m_filePath.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.55f, 0.62f, 0.72f, 1.0f), "Selected: %s", m_filePath.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const float btnW = (winW - 60.0f) * 0.5f;

        ImGui::SetCursorPosX(24.0f);
        if (bigButton("< Back", btnW)) {
            m_filePath.clear();
            m_screen = Screen::Main;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(winW - btnW - 24.0f);

        bool enableLoad = !m_filePath.empty();
        if (!enableLoad) ImGui::BeginDisabled();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.40f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.55f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.30f, 0.08f, 1.0f));
        if (bigButton("Load >", btnW)) {
            m_loadPath = m_filePath;
            out = Action::LoadWorld;
        }
        ImGui::PopStyleColor(3);
        if (!enableLoad) ImGui::EndDisabled();
    }
    ImGui::End();
}

// ─── Generowanie świata (pasek postępu + komunikaty) ─────────────────────────
void StartScreen::renderGenerating(Action& out) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const float winW = 360.0f;
    const float winH = 140.0f;
    ImGui::SetNextWindowPos({ (sw - winW) * 0.5f, (sh - winH) * 0.5f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ winW, winH }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.93f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (m_genMessages.empty()) {
        m_genMessages = {
            "Generating terrain...",
            "Placing craters...",
            "Adding ice deposits...",
            "Calculating slopes...",
            "Populating resources...",
            "Finalizing world..."
        };
    }

    float elapsed = m_time - m_genStartTime;
    float progress = elapsed / 3.0f;
    if (progress > 1.0f) progress = 1.0f;

    int msgIdx = (int)(progress * m_genMessages.size());
    if (msgIdx >= (int)m_genMessages.size()) msgIdx = (int)m_genMessages.size() - 1;
    const std::string& currentMsg = m_genMessages[msgIdx];

    if (ImGui::Begin("##generating", nullptr, flags)) {
        ImGui::SetWindowFontScale(1.3f);
        ImGui::TextColored(ImVec4(0.10f, 0.70f, 0.95f, 1.0f), "Generating World...");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.86f, 0.92f, 1.0f, 1.0f), "%s", currentMsg.c_str());
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.10f, 0.70f, 0.95f, 1.0f));
        ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
        ImGui::PopStyleColor();
    }
    ImGui::End();

    if (progress >= 1.0f) {
        out = Action::CreateWorld;
        m_genMessages.clear();
        m_screen = Screen::Main;  // zostanie nadpisane przez główną pętlę
    }
}

// ─── Apply settings ───────────────────────────────────────────────────────────
void StartScreen::applySettings() {
    WorldGenSettings::applyMapSizePreset(m_mapSizeIdx, m_settings);
    m_settings.noiseScale = 0.02f + m_roughness * 0.10f;
    m_settings.maxHeight = 4 + (int)(m_elevation * 12.0f);
    m_settings.lakeThreshold = 0.8f - m_lakeFreq * 0.5f;
    m_settings.seed = m_randomSeed ? (rand() % 100000) : m_seed;
}
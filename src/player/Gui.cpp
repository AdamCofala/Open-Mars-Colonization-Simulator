#include "Gui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "world/World.h"
#include "rendering/Renderer.h"
#include "entities/Enums.h"

#include "structures/SolarPanel.h"
#include "entities/Pipe.h"
#include "structures/IceMelter.h"

#include <string>
#include <vector>
#include <algorithm>

namespace {
    struct BuildingUIData {
        std::string name;
        std::string description;
        std::string detailedInfo;
        std::string textureKey;
    };

    std::vector<BuildingUIData> s_BuildingData;

    void InitBuildingData() {
        if (!s_BuildingData.empty()) return;

        SolarPanel panel(0, 0);
        Pipe pipe(0, 0);
        IceMelter melter(0, 0);

        s_BuildingData.resize(3);
        s_BuildingData[0] = { "Solar Panel",
                              "Generates energy from sunlight.",
                              "Produces: 2 Energy/s\nNo consumption",
                              panel.getTextureId() };
        s_BuildingData[1] = { "Pipe",
                              "Transfers resources between structures.",
                              "No production/consumption\nConnects to adjacent pipes and ports.",
                              pipe.getTextureId() };
        s_BuildingData[2] = { "Ice Melter",
                              "Melts ice into water.",
                              "Consumes: 1 Energy/s\nProduces: 1 Water/s",
                              melter.getTextureId() };
    }

    const BuildingUIData& GetBuildingData(int type) {
        return s_BuildingData[type];
    }
}

ImVec4 Gui::SelectedColor(bool selected) const {
    return selected ? ImVec4(0.12f, 0.45f, 0.8f, 1.0f) : ImVec4(0.12f, 0.2f, 0.32f, 1.0f);
}

void Gui::init(World* world, Renderer* renderer) {
    m_world = world;
    m_renderer = renderer;
    rlImGuiBeginInitImGui();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/CascadiaCode.ttf", 17.0f);
    if (!font) font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 17.0f);
    if (!font) io.Fonts->AddFontDefault();

    io.FontGlobalScale = 1.0f;
    rlImGuiEndInitImGui();

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.WindowPadding = ImVec2(12.0f, 10.0f);
    style.FramePadding = ImVec2(10.0f, 7.0f);
    style.ItemSpacing = ImVec2(10.0f, 9.0f);
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.TabBorderSize = 1.0f;

    style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.92f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.62f, 0.72f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.05f, 0.08f, 0.95f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.05f, 0.08f, 0.88f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.10f, 0.70f, 0.95f, 0.35f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.02f, 0.05f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.03f, 0.10f, 0.16f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.02f, 0.05f, 0.09f, 0.95f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.11f, 0.16f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.18f, 0.26f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.22f, 0.32f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.07f, 0.14f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.32f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.40f, 0.55f, 1.0f);

    style.Colors[ImGuiCol_Header] = ImVec4(0.08f, 0.18f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 0.30f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.38f, 0.55f, 1.0f);

    style.Colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.70f, 0.95f, 0.60f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.70f, 0.95f, 0.85f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.70f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.10f, 0.70f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.10f, 0.70f, 0.95f, 0.85f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.10f, 0.78f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.10f, 0.22f, 0.32f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.32f, 0.44f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.40f, 0.55f, 1.0f);

    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.10f, 0.70f, 0.95f, 0.25f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.10f, 0.70f, 0.95f, 0.60f);
}

void Gui::render() {
    rlImGuiBegin();

    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());

    // ---------- MENU GÓRNE ----------
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            ImGui::MenuItem("Save");
            ImGui::MenuItem("Load");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Speed")) {
            ImGui::Text("Game Speed");
            ImGui::SetNextItemWidth(140.0f);
            ImGui::SliderFloat("##speed", &m_gameSpeed, 0.5f, 4.0f, "x%.1f");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Overview")) {
                if (m_renderer && m_world) {
                    Vector2 centerIso = m_renderer->IsoToScreen(
                        m_world->getMap().getHalfWidth(),
                        m_world->getMap().getHalfHeight(),
                        &m_world->getMap());
                    m_renderer->getGameCamera().setTarget(centerIso);
                    m_renderer->getGameCamera().setZoom(m_renderer->getGameCamera().getMinZoom(), false);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                m_showAboutWindow = true;
            }
            ImGui::EndMenu();
        }
        if (m_world) {
            std::string dateText = "Day " + std::to_string(m_world->getDay()) +
                " - Sol " + std::to_string(m_world->getMonth()) +
                " - " + std::to_string(m_world->getYear());
            float textWidth = ImGui::CalcTextSize(dateText.c_str()).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - textWidth - 20.0f);
            ImGui::TextUnformatted(dateText.c_str());
        }
        ImGui::EndMainMenuBar();
    }

    // ---------- OKNO BUDOWANIA ----------
    ImGui::SetNextWindowPos(ImVec2(screenWidth - 20.0f, 40.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::Begin("Build Menu", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);

    ImGui::Text("Resources");
    ImGui::Separator();
    if (m_world) {
        float energyTotal = m_world->getTotalEnergy();
        float energyCapacity = m_world->getTotalEnergyCapacity();
        float ratio = energyCapacity > 0.0f ? (energyTotal / energyCapacity) : 0.0f;
        ImGui::Text("Energy: %.2f / %.2f", energyTotal, energyCapacity);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.98f, 0.8f, 0.2f, 1.0f));
        ImGui::ProgressBar(ratio, ImVec2(-1.0f, 0.0f));
        ImGui::PopStyleColor();

        ImGui::Spacing();
        float waterTotal = m_world->getTotalWater();
        float waterCapacity = m_world->getTotalWaterCapacity();
        float waterRatio = waterCapacity > 0.0f ? (waterTotal / waterCapacity) : 0.0f;
        ImGui::Text("Water: %.2f / %.2f", waterTotal, waterCapacity);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.10f, 0.60f, 0.95f, 1.0f));
        ImGui::ProgressBar(waterRatio, ImVec2(-1.0f, 0.0f));
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Text("Structures");
    ImGui::Separator();

    InitBuildingData();
    const TextureManager& texMan = m_renderer->getTextureManager();

    for (int i = 0; i < 3; i++) {
        const BuildingUIData& data = GetBuildingData(i);
        const Texture2D& tex = texMan.StuctureTexturesInfo.at(data.textureKey);

        // proporcje ikony (bez zmian)
        float maxIconWidth = 64.0f;
        float maxIconHeight = 31.0f;
        float aspect = (float)tex.width / (float)tex.height;
        ImVec2 iconSize;
        if (aspect > 1.0f) {
            iconSize.x = maxIconWidth;
            iconSize.y = maxIconWidth / aspect;
        }
        else {
            iconSize.y = maxIconHeight;
            iconSize.x = maxIconHeight * aspect;
        }
        iconSize.x = std::max(iconSize.x, 20.0f);
        iconSize.y = std::max(iconSize.y, 20.0f);

        // Ikona jako przycisk
        ImGui::PushID(i);
        if (ImGui::ImageButton("##buildingbtn", (ImTextureID)(intptr_t)tex.id, iconSize)) {
            m_selectedTool = SelectedTool::Build;
            m_selectedBuilding = i;
        }
        ImGui::PopID();

        ImGui::SameLine(0, 6); // ma³y odstêp miêdzy ikon¹ a nazw¹

        // Selectable bez podawania rozmiaru – rozci¹gnie siê na ca³¹ dostêpn¹ szerokoœæ
        bool selected = (m_selectedTool == SelectedTool::Build && m_selectedBuilding == i);
        if (ImGui::Selectable(data.name.c_str(), selected)) {
            m_selectedTool = SelectedTool::Build;
            m_selectedBuilding = i;
        }

        // Tooltip (bez zmian)
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.0f);
            ImGui::TextUnformatted(data.description.c_str());
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextUnformatted(data.detailedInfo.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    ImGui::End();

    // ---------- PASEK NARZÊDZI ----------
    ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.5f, screenHeight - 15.0f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, SelectedColor(m_selectedTool == SelectedTool::Select));
    if (ImGui::Button("Select")) {
        m_selectedTool = SelectedTool::Select;
        m_selectedBuilding = -1;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, SelectedColor(m_selectedTool == SelectedTool::Build));
    if (ImGui::Button("Build")) {
        m_selectedTool = SelectedTool::Build;
        if (m_selectedBuilding < 0) {
            m_selectedBuilding = 0;
        }
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, SelectedColor(m_selectedTool == SelectedTool::Demolish));
    if (ImGui::Button("Demolish")) {
        m_selectedTool = SelectedTool::Demolish;
        m_selectedBuilding = -1;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();

    // ---------- OKNO ABOUT ----------
    if (m_showAboutWindow) {
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.5f, screenHeight * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("About Open Mars", &m_showAboutWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Open Mars Colonization Simulator");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("A modern, open-source game about colonizing Mars.\nSmart horse lives in car.");
            ImGui::Spacing();
            ImGui::Text("Created by:");
            ImGui::BulletText("Adam Cofala @CoreOfChaos");
            ImGui::BulletText("Jakub Salata @JLetuce");
            ImGui::BulletText("Jedrzej Tyminski @Kayla137");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled("Version 1.0.0-alpha.1");
        }
        ImGui::End();
    }

    rlImGuiEnd();
}
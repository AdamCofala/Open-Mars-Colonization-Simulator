#include "Gui.h"
#include "raylib.h"
#include "world/World.h"
#include "entities/MaterialType.h"

namespace {
	ImVec4 SelectedColor(bool selected) {
		return selected ? ImVec4(0.12f, 0.45f, 0.8f, 1.0f) : ImVec4(0.12f, 0.2f, 0.32f, 1.0f);
	}
}

void Gui::init(World* world)
{
	m_world = world;
 rlImGuiBeginInitImGui();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/CascadiaCode.ttf", 16.0f);
	rlImGuiEndInitImGui();

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();
    style.WindowRounding = 1.0f;
	style.FrameRounding = 1.0f;
	style.GrabRounding = 1.0f;
	style.ScrollbarRounding = 1.0f;
	style.FramePadding = ImVec2(10.0f, 6.0f);
	style.ItemSpacing = ImVec2(8.0f, 8.0f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.08f, 0.12f, 0.95f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.07f, 0.12f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.12f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.14f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.2f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.26f, 0.38f, 1.0f);
   style.Colors[ImGuiCol_Button] = ImVec4(0.08f, 0.18f, 0.32f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.12f, 0.3f, 0.48f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.38f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.22f, 0.34f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.32f, 0.46f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.38f, 0.54f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	style.WindowBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.Colors[ImGuiCol_Separator] = ImVec4(0.1f, 0.55f, 0.9f, 1.0f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.8f, 0.9f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2f, 0.45f, 0.7f, 0.6f);
}

void Gui::render()
{
	rlImGuiBegin();

	const float screenWidth = static_cast<float>(GetScreenWidth());
	const float screenHeight = static_cast<float>(GetScreenHeight());

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
			ImGui::MenuItem("Overview");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			ImGui::MenuItem("About");
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

 ImGui::SetNextWindowPos(ImVec2(screenWidth - 20.0f, 30.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
	ImGui::Begin("Build Menu", nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

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
	}

	ImGui::Spacing();
	ImGui::Text("Structures");
	ImGui::Separator();
	bool buildSelected = m_selectedTool == SelectedTool::Build && m_selectedBuilding == 0;
	ImGui::PushStyleColor(ImGuiCol_Button, SelectedColor(buildSelected));
	if (ImGui::Button("Solar Panel", ImVec2(-1.0f, 0.0f))) {
		m_selectedTool = SelectedTool::Build;
		m_selectedBuilding = 0;
	}
	ImGui::PopStyleColor();
	ImGui::End();

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

	rlImGuiEnd();
}

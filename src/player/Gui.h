#pragma once
#include "imgui.h"
#include "rlImGui.h"
#include <string>

class World;
class Renderer;

class Gui
{
public:
	enum class SelectedTool {
		Select,
		Build,
		Terraform,
		Demolish
	};

	Gui() = default;
	void init(World* world, Renderer* renderer);
	void render();
	void shutdown() { rlImGuiShutdown(); }

	SelectedTool getSelectedTool() const { return m_selectedTool; }
	void setSelectedTool(SelectedTool tool) { m_selectedTool = tool; }
	int getSelectedBuilding() const { return m_selectedBuilding; }
	float getGameSpeed() const { return m_gameSpeed; }

private:
	World* m_world = nullptr;
	Renderer* m_renderer = nullptr;
	SelectedTool m_selectedTool = SelectedTool::Select;
	int m_selectedBuilding = -1;
	float m_gameSpeed = 1.0f;
	bool m_showAboutWindow = false;
	ImVec4 SelectedColor(bool selected) const;
};

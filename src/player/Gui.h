#pragma once
#include "imgui.h"
#include "rlImGui.h"

class World;

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
	void init(World* world);
	void render();
	void shutdown() { rlImGuiShutdown(); }

	SelectedTool getSelectedTool() const { return m_selectedTool; }
	int getSelectedBuilding() const { return m_selectedBuilding; }
	float getGameSpeed() const { return m_gameSpeed; }

private:
	World* m_world = nullptr;
	SelectedTool m_selectedTool = SelectedTool::Select;
	int m_selectedBuilding = -1;
  float m_gameSpeed = 1.0f;
};

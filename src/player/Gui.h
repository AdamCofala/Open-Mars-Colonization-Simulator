#pragma once
#include "imgui.h"
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

	// Metoda shutdown nie dotyka ju¿ rlImGuiShutdown()
	void shutdown() {}

	SelectedTool getSelectedTool() const { return m_selectedTool; }
	void setSelectedTool(SelectedTool tool) { m_selectedTool = tool; }
	int getSelectedBuilding() const { return m_selectedBuilding; }
	float getGameSpeed() const { return m_gameSpeed; }

	bool shouldExitToMenu() const { return m_exitToMenu; }
	void resetExitToMenu() { m_exitToMenu = false; }

private:
	World* m_world = nullptr;
	Renderer* m_renderer = nullptr;
	SelectedTool m_selectedTool = SelectedTool::Select;
	int m_selectedBuilding = -1;
	float m_gameSpeed = 1.0f;
	bool m_showAboutWindow = false;
	bool m_exitToMenu = false;

	ImVec4 SelectedColor(bool selected) const;
};
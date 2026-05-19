#include "imgui.h"
#include "rlImGui.h"


class Gui
{
public:
	Gui() = default;
	void init();
	void render();
	void shutdown() { rlImGuiShutdown(); }
};
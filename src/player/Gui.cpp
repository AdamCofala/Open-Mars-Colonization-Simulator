#include "Gui.h"


void Gui::init()
{
	rlImGuiSetup(true);
}


void Gui::render()
{
	static bool showDemoWindow = true;
	rlImGuiBegin();
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
	rlImGuiEnd();
}

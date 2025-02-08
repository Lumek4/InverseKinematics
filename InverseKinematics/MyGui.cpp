#include "MyGui.h"
#include"imgui\imgui.h"
#include"imgui\imgui_internal.h"

bool MyGui::IsWindowMoving()
{
	return ImGui::GetCurrentContext()->MovingWindow == ImGui::GetCurrentWindow();
}

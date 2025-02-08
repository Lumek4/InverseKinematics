#pragma once
#include"imgui\imgui.h"
inline static ImVec2 uniformToCanvas(ImVec2 a, ImVec2 min, ImVec2 extent)
{
	a.x = (a.x + 1) * 0.5f * extent.x + min.x;
	a.y = (a.y + 1) * 0.5f * extent.y + min.y;
	return a;
}
inline static ImVec2 canvasToUniform(ImVec2 a, ImVec2 min, ImVec2 extent)
{
	a.x = (a.x - min.x) / extent.x * 2 - 1;
	a.y = (a.y - min.y) / extent.y * 2 - 1;
	return a;
}
inline static ImVec2 uniformToCanvas(ImVec2 a)
{
	ImVec2 min = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 extent = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
	a.x = (a.x + 1) * 0.5f * extent.x + min.x;
	a.y = (a.y + 1) * 0.5f * extent.y + min.y;
	return a;
}
inline static ImVec2 canvasToUniform(ImVec2 a)
{
	ImVec2 min = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 extent = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
	a.x = (a.x - min.x) / extent.x * 2 - 1;
	a.y = (a.y - min.y) / extent.y * 2 - 1;
	return a;
}

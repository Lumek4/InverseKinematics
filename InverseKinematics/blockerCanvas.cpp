#include "blockerCanvas.h"
#include"imgui\imgui.h"
#include"vecmath.h"
#include"MyGui.h"
#include"canvasUtilities.h"
using namespace DirectX;

void BlockerCanvas::Draw()
{
	ImGui::Begin("Canvas");
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 regionMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 regionExtent = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();

	for (int i = 0; i < m_aaRects.size(); i++)
	{
		auto new_a = uniformToCanvas(m_aaRects[i].a, regionMin, regionExtent);
		auto new_b = uniformToCanvas(m_aaRects[i].b, regionMin, regionExtent);
		drawList->AddRectFilled(new_a, new_b,
			ImGui::GetColorU32(m_color));
		if (m_editing == i)
			drawList->AddRect(new_a, new_b,-1);
	}
	if (m_currentlyDrawing)
	{
		auto new_a = uniformToCanvas(
			{ fminf(m_drawStart.x, m_drawEnd.x), fminf(m_drawStart.y, m_drawEnd.y) },
			regionMin, regionExtent);
		auto new_b = uniformToCanvas(
			{ fmaxf(m_drawStart.x, m_drawEnd.x), fmaxf(m_drawStart.y, m_drawEnd.y) },
			regionMin, regionExtent);
		auto dimmed = m_color;
		dimmed.w *= 0.5;
		drawList->AddRectFilled(new_a, new_b,
			ImGui::GetColorU32(dimmed));
	}

	drawList->AddLine(
		uniformToCanvas({ -1,0 }, regionMin, regionExtent),
		uniformToCanvas({ 1,0 }, regionMin, regionExtent), -1);
	drawList->AddLine(
		uniformToCanvas({ 0,-1 }, regionMin, regionExtent),
		uniformToCanvas({ 0,1 }, regionMin, regionExtent), -1);
	ImGui::End(); // Canvas
}

void BlockerCanvas::UpdateControls()
{
	if (m_locked)
		return;
	ImGui::Begin("Canvas");

	ImVec2 regionMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 regionExtent = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
	ImVec2 mouseUniform = canvasToUniform(ImGui::GetMousePos(), regionMin, regionExtent);

	if (ImGui::GetIO().WantCaptureMouse && ImGui::IsWindowHovered() &&
		ImGui::IsMouseDown(1))
	{
		m_editing = -1;
		for (int i = m_aaRects.size()-1; i >= 0; i--)
		{
			if (vecmath::bb2d(m_aaRects[i].a, m_aaRects[i].b, mouseUniform))
			{
				m_editing = i;
				break;
			}
		}
	}
	if (m_editing >= 0 && ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		m_aaRects.erase(m_aaRects.begin() + m_editing);
		m_editing = -1;
	}

	if (ImGui::GetIO().WantCaptureMouse && ImGui::IsWindowHovered() &&
		ImGui::IsMouseDown(0) && !ImGui::IsMouseDragging(0) &&
		!m_currentlyDrawing)
	{
		if (vecmath::bb2d({ -1,-1 }, { 1,1 }, mouseUniform))
		{
			m_drawStart = m_drawEnd = mouseUniform;
			m_currentlyDrawing = true;
		}
	}
	else if (m_currentlyDrawing)
	{
		m_drawEnd = mouseUniform;
		if (!ImGui::IsMouseDown(0) && vecmath::length(m_drawEnd-m_drawStart)>1e-5)
		{
			m_currentlyDrawing = false;
			m_aaRects.push_back({
				{fminf(m_drawStart.x, m_drawEnd.x), fminf(m_drawStart.y, m_drawEnd.y)},
				{fmaxf(m_drawStart.x, m_drawEnd.x), fmaxf(m_drawStart.y, m_drawEnd.y)}
				});
		}
	}
	ImGui::End(); // Canvas
}

void BlockerCanvas::GUI()
{
	ImGui::Begin("Canvas Options");
	ImGui::ColorPicker3("Blocker Color", &m_color.x);
	ImGui::BeginGroup();
	{
		char label[256];
		int deleting = -1;
		for (int i = 0; i < m_aaRects.size(); i++)
		{
			sprintf_s(label, "X##Box%d", i);
			if (ImGui::Button(label))
				deleting = i;

			sprintf_s(label, "Box##%d", i);
			bool selected = i == m_editing;
			ImGui::SameLine();
			ImGui::Selectable(label, &selected);
			if (selected)
				m_editing = i;
		}
		if (deleting >= 0)
		{
			m_aaRects.erase(m_aaRects.begin() + deleting);
			if (deleting < m_editing)
				m_editing--;
			else if (deleting == m_editing)
				m_editing = -1;
		}
	}
	ImGui::EndGroup();
	ImGui::End(); // Canvas Options
}

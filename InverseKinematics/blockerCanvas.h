#pragma once
#include<vector>
#include"vecmath.h"
#include"imgui\imgui.h"

class BlockerCanvas
{
public:
	void Draw();
	void UpdateControls();
	void GUI();
	std::vector<AARect> m_aaRects{};
	bool m_locked = false;
private:
	ImVec4 m_color{.5,.2,1,1};


	ImVec2 m_drawStart = {};
	ImVec2 m_drawEnd = {};
	bool m_currentlyDrawing = false;

	int m_editing = -1;
};
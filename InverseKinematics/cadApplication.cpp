#include "cadApplication.h"
#include<windowsx.h>
#include"imgui/imgui_impl_dx11.h"
#include"imgui/imgui_impl_win32.h"
#include"vecmath.h"
#include<chrono>
#include"MyGui.h"
#include"cyclic_buffer_static.h"
#include"cyclic_buffer_dynamic.h"
using namespace mini;
using namespace DirectX;
using namespace std::chrono_literals;

CadApplication::CadApplication(HINSTANCE hInstance)
	:DxApplication(hInstance),
	m_frameTimer{},
	m_frameTimes{ new cyclic_buffer_static<double,512>{} }
{
	m_kinematicChain.m_obstacles = &m_blockerCanvas.m_aaRects;
	OnResize();

}

CadApplication::~CadApplication()
{
}

double summator(double v, float t);
void CadApplication::GUI()
{
	auto viewportDock = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	auto& io = ImGui::GetIO();
	m_blockerCanvas.GUI();
	m_kinematicChain.GUI();
	ImGui::Begin("General");
	{
		double time = m_frameTimes->sum(summator);
		ImGui::Text("Visual:    %4.1lf FPS", 1 / time);
		if (!m_blockerCanvas.m_locked)
			ImGui::Text("Drawing Blockers");
		else if (ImGui::Button("Drawing Blockers"))
		{
			m_blockerCanvas.m_locked = false;
			m_kinematicChain.m_locked = true;
		}
		ImGui::SameLine();
		if (!m_kinematicChain.m_locked)
			ImGui::Text("Selecting Positions");
		else if (ImGui::Button("Selecting Positions"))
		{
			m_blockerCanvas.m_locked = true;
			m_kinematicChain.m_locked = false;
		}
	}
	ImGui::End(); // General

}

void CadApplication::Update() {
	auto& io = ImGui::GetIO();
	m_blockerCanvas.UpdateControls();
	m_kinematicChain.UpdateControls();
	m_frameTimer.lap();
	m_frameTimes->push(m_frameTimer.getDeltaTime());
}


void CadApplication::OnResize()
{
	auto wndSize = m_window.getClientSize();
	float ratio = static_cast<float>(wndSize.cx) / wndSize.cy;
}

void CadApplication::Render()
{
	float color[4] = { 0,0,0,1 };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), color);
	m_blockerCanvas.Draw();
	m_kinematicChain.DrawConfigSpace();
	m_kinematicChain.Draw();
}

double summator(double v, float t)
{
	float cpoints[4] = { 1, 1, 0, 0 };
	float b3[4], b2[3], b1[2];

	float mt = 1 - t;
	b1[0] = mt;
	b1[1] = t;

	b2[0] = b1[0] * mt;
	b2[1] = b1[0] * t + b1[1] * mt;
	b2[2] = b1[1] * t;

	b3[0] = b2[0] * mt;
	b3[1] = b2[0] * t + b2[1] * mt;
	b3[2] = b2[1] * t + b2[2] * mt;
	b3[3] = b2[2] * t;

	float s = 0;
	for (int i = 0; i < 4; i++)
		s += b3[i] * cpoints[i];
	return v*s*2;
}
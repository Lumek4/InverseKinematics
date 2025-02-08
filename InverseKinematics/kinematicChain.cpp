#include "kinematicChain.h"
#include"MyGui.h"
#include"imgui\imgui.h"
#include"canvasUtilities.h"
#include <queue>
using namespace DirectX;
inline uint32_t KinematicChain::GetColor(int16_t value)
{
	uint32_t one = 0xff;
	if (value < 0)
		return one * 0x100 + one * 0x1000000;
	if (value == 0)
		return 0xff7f7f7f;
	return (255 - value * 255 / m_steps) + one * 0x1000000;
}

KinematicChain::KinematicChain()
{
}
void KinematicChain::DrawConfigSpace()
{
	ImGui::Begin("Configuration Space");
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 regionMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 regionExtent = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();

	if (!m_startSet || !m_endSet)
		ImGui::BeginDisabled();
	if (ImGui::Button("Find Path!", regionExtent))
		FindPath();
	if (!m_startSet || !m_endSet)
		ImGui::EndDisabled();
	if (!m_isConfigSpaceCalculated)
	{
		ImGui::End();
		return;
	}
	for (int y = 0; y < c_configSpaceSize; y++)
	{
		float yStart = regionMin.y + y * regionExtent.y / c_configSpaceSize;
		float yEnd = regionMin.y + (y+1) * regionExtent.y / c_configSpaceSize;
		float xStart = regionMin.x;
		int16_t value = m_configSpace[y * c_configSpaceSize];
		for (int x = 1; x < c_configSpaceSize; x++)
		{
			if (m_configSpace[x + y * c_configSpaceSize] == value)
				continue;
			float xEnd = regionMin.x + x * regionExtent.x / c_configSpaceSize;
			drawList->AddRectFilled({xStart, yStart}, {xEnd, yEnd}, GetColor(value));
			xStart = xEnd;
			value = m_configSpace[x + y * c_configSpaceSize];
		}
		float xEnd = regionMin.x + regionExtent.x;
		drawList->AddRectFilled({ xStart, yStart }, { xEnd, yEnd }, GetColor(value));
	}
	if (m_isAnimating)
		drawList->AddCircleFilled({
			regionMin.x + m_r_animation.x * regionExtent.x / c_configSpaceSize,
			regionMin.y + m_r_animation.y * regionExtent.y / c_configSpaceSize
			}, 5, 0xffff3f00);
	if (!m_foundStart)
	{
		drawList->AddLine(
			uniformToCanvas({ -1,-1 }),
			uniformToCanvas({ 1,1 }), 0xff0000ff, 5);
		drawList->AddLine(
			uniformToCanvas({ 1,-1 }),
			uniformToCanvas({ -1,1 }), 0xff0000ff, 5);
	}
	ImGui::End(); // Configuration Space
}

void KinematicChain::Draw()
{
	ImGui::Begin("Canvas");
	auto drawList = ImGui::GetWindowDrawList();


	if (m_startSet)
	{
		drawList->AddLine(
			uniformToCanvas({}),
			uniformToCanvas(ForwardKinematics(m_r_start.x)), 0xffff3f7f);
		drawList->AddLine(
			uniformToCanvas(ForwardKinematics(m_r_start.x)),
			uniformToCanvas(ForwardKinematics(m_r_start)), 0xffff3f7f);
	}
	if (m_endSet)
	{
		drawList->AddLine(
			uniformToCanvas({}),
			uniformToCanvas(ForwardKinematics(m_r_end.x)), 0xff3fff7f);
		drawList->AddLine(
			uniformToCanvas(ForwardKinematics(m_r_end.x)),
			uniformToCanvas(ForwardKinematics(m_r_end)), 0xff3fff7f);
	}
	if (!m_locked)
	{
		if (!IsCollidingAny(m_r_solution1))
		{
			drawList->AddLine(
				uniformToCanvas({}),
				uniformToCanvas(ForwardKinematics(m_r_solution1.x)), 0xff7f7fff);
			drawList->AddLine(
				uniformToCanvas(ForwardKinematics(m_r_solution1.x)),
				uniformToCanvas(ForwardKinematics(m_r_solution1)), 0xff7f7fff);
		}
		if (!IsCollidingAny(m_r_solution2))
		{
			drawList->AddLine(
				uniformToCanvas({}),
				uniformToCanvas(ForwardKinematics(m_r_solution2.x)), 0xffff7f7f);
			drawList->AddLine(
				uniformToCanvas(ForwardKinematics(m_r_solution2.x)),
				uniformToCanvas(ForwardKinematics(m_r_solution2)), 0xffff7f7f);
		}
	}


	if (m_isAnimating)
	{
		drawList->AddLine(
			uniformToCanvas({}),
			uniformToCanvas(ForwardKinematics(m_r_animation.x)), -1);
		drawList->AddLine(
			uniformToCanvas(ForwardKinematics(m_r_animation.x)),
			uniformToCanvas(ForwardKinematics(m_r_animation)), -1);
		//if (m_animationSkip++ > c_animation_rate)
		{
			m_animationSkip = 0;
			XMINT2 c[8];
			c[0] = XMINT2{ m_r_animation.x + 0, m_r_animation.y + 1 };
			c[1] = XMINT2{ m_r_animation.x + 1, m_r_animation.y + 0 };
			c[2] = XMINT2{ m_r_animation.x + 0, m_r_animation.y - 1 };
			c[3] = XMINT2{ m_r_animation.x - 1, m_r_animation.y + 0 };
			c[4 + 0] = XMINT2{ m_r_animation.x + 1, m_r_animation.y + 1 };
			c[4 + 1] = XMINT2{ m_r_animation.x + 1, m_r_animation.y - 1 };
			c[4 + 2] = XMINT2{ m_r_animation.x - 1, m_r_animation.y - 1 };
			c[4 + 3] = XMINT2{ m_r_animation.x - 1, m_r_animation.y + 1 };
			int16_t minv = m_steps + 1, mini = -1;
			for (int i = 0; i < (m_isEuclidean ? 8 : 4); i++)
			{
				c[i].x += c_configSpaceSize; c[i].x %= c_configSpaceSize;
				c[i].y += c_configSpaceSize; c[i].y %= c_configSpaceSize;
				int16_t cv = m_configSpace[c[i].x + c[i].y * c_configSpaceSize];
				if (cv > 0 && cv < minv)
				{
					minv = cv; mini = i;
				}
			}
			m_r_animation = c[mini];
			if (m_r_animation == m_r_end)
				m_isAnimating = false;
		}
	}


	ImGui::End(); // Canvas
}

void KinematicChain::UpdateControls()
{
	if (m_locked)
		return;
	ImGui::Begin("Canvas");
	m_p_effector = canvasToUniform(ImGui::GetMousePos());
	auto sols = InverseKinematics(m_p_effector);
	m_r_solution1 = { sols.x, sols.y };
	m_r_solution2 = { sols.z, sols.w };
	m_viable1 = !IsCollidingAny(m_r_solution1);
	m_viable2 = !IsCollidingAny(m_r_solution2);

	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && ImGui::GetIO().WantCaptureMouse)
	{
		if (!m_startSet)
		{
			m_p_start = m_p_effector;
			m_r_start1 = m_r_solution1;
			m_r_start2 = m_r_solution2;
			m_startSet = true;

			if (m_viable1)
				m_r_start = m_r_start1;
			else if (m_viable2)
				m_r_start = m_r_start2;
			else
				m_startSet = false;

			m_startFlippable = m_viable1 == m_viable2;
		}
		else if (!m_endSet)
		{
			m_p_end = m_p_effector;
			m_r_end1 = m_r_solution1;
			m_r_end2 = m_r_solution2;
			m_endSet = true;

			if (m_viable1)
				m_r_end = m_r_end1;
			else if (m_viable2)
				m_r_end = m_r_end2;
			else
				m_endSet = false;

			m_endFlippable = m_viable1 == m_viable2;
		}
	}

	ImGui::End(); // Canvas
}

void KinematicChain::GUI()
{
	ImGui::Begin("Pathfinding Parameters");
	if (ImGui::DragFloat2("Arm Lengths", &m_lengths.x, 0.01, EPS, 2, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		m_startSet = m_endSet = false;
		m_isConfigSpaceCalculated = false;
	}
	
	if (m_locked)
		ImGui::BeginDisabled();
	if (!m_startSet)
		ImGui::Text("Start Unset...");
	else if (ImGui::Button("Set New Start"))
	{
		m_startSet = false;
		m_isConfigSpaceCalculated = false;
	}
	ImGui::SameLine();
	if (!m_endSet)
		ImGui::Text("End Unset...");
	else if (ImGui::Button("Set New End"))
	{
		m_endSet = false;
		m_isConfigSpaceCalculated = false;
	}
	if (m_locked)
		ImGui::EndDisabled();

	if (!m_startSet || !m_startFlippable)
		ImGui::BeginDisabled();
	if (ImGui::Button("Flip Start"))
	{
		m_r_start = (m_r_start == m_r_start1) ? m_r_start2 : m_r_start1;
		m_isConfigSpaceCalculated = false;
	}

	if (!m_startSet || !m_startFlippable)
		ImGui::EndDisabled();
	ImGui::SameLine();

	if (!m_endSet || !m_endFlippable)
		ImGui::BeginDisabled();
	if (ImGui::Button("Flip End"))
	{
		m_r_end = (m_r_end == m_r_end1) ? m_r_end2 : m_r_end1;
		m_isConfigSpaceCalculated = false;
	}
	if (!m_endSet || !m_endFlippable)
		ImGui::EndDisabled();

	if (!m_isConfigSpaceCalculated || !m_foundStart)
		ImGui::BeginDisabled();
	if (ImGui::Button("Animate"))
	{
		m_r_animation = m_r_start;
		m_isAnimating = true;
	}
	if (!m_isConfigSpaceCalculated || !m_foundStart)
		ImGui::EndDisabled();
	ImGui::End(); // Pathfinding Parameters
}

DirectX::XMFLOAT2 KinematicChain::ForwardKinematics(float angle)
{
	DirectX::XMFLOAT2 v;
	XMStoreFloat2(&v,
		XMVector3Transform(
			XMVectorZero(),
			XMMatrixTranslation(m_lengths.x, 0, 0)*
			XMMatrixRotationZ(XMConvertToRadians(angle))
		
		)
	);
	return v;
}

DirectX::XMFLOAT2 KinematicChain::ForwardKinematics(DirectX::XMINT2 angles)
{
	DirectX::XMFLOAT2 v;
	XMStoreFloat2(&v,
		XMVector3Transform(
			XMVectorZero(),
			XMMatrixTranslation(m_lengths.y, 0, 0)
			* XMMatrixRotationZ(XMConvertToRadians(angles.y))
			* XMMatrixTranslation(m_lengths.x, 0, 0)
			* XMMatrixRotationZ(XMConvertToRadians(angles.x))
		)
	);
	return v;
}

DirectX::XMINT4 KinematicChain::InverseKinematics(DirectX::XMFLOAT2 position)
{
	float distance = vecmath::length(position);
	XMFLOAT2 sol1, sol2;
	if (distance >= m_lengths.x + m_lengths.y || distance <= fabsf(m_lengths.x - m_lengths.y)) {

		sol1.x = sol2.x = atan2(position.y, position.x);
		sol1.y = sol2.y = 0;
		m_p_effector = position * (m_lengths.x + m_lengths.y) * (1 / distance);

		float actualDis = abs(distance - (m_lengths.x + m_lengths.y));

		if (abs(distance - (m_lengths.x - m_lengths.y)) < actualDis) {
			sol1.y = sol2.y = XM_PI;
			actualDis = abs(distance - (m_lengths.x - m_lengths.y));
		}
		if (abs(distance - (-m_lengths.x + m_lengths.y)) < actualDis) {
			sol1.x = sol2.x = XM_PI + sol1.x;
			sol1.y = sol2.y = XM_PI;
		}
		if (sol1.x < 0)
			sol1.x += XM_2PI;
		if (sol2.x < 0)
			sol2.x += XM_2PI;
		return {
			(int)XMConvertToDegrees(sol1.x),
			(int)XMConvertToDegrees(sol1.y),
			(int)XMConvertToDegrees(sol2.x),
			(int)XMConvertToDegrees(sol2.y) };
	}

	float y = acosf((vecmath::lengthSq(m_lengths) - distance*distance) / (2.0f * m_lengths.x * m_lengths.y));
	sol1.y = XM_PI + y;
	sol2.y = XM_PI - y;
	if (sol1.y < 0)
		sol1.y += XM_2PI;
	if (sol2.y < 0)
		sol2.y += XM_2PI;

	XMMATRIX m = XMMatrixTranslation(m_lengths.y,0,0) * XMMatrixRotationZ(sol1.y) * XMMatrixTranslation(m_lengths.x, 0, 0);
	XMFLOAT2 g;
	XMStoreFloat2(&g, XMVector3Transform({}, m));
	sol1.x = atan2f(-g.y * position.x + g.x * position.y, g.x * position.x + g.y * position.y);

	m = XMMatrixTranslation(m_lengths.y, 0, 0) * XMMatrixRotationZ(sol2.y) * XMMatrixTranslation(m_lengths.x, 0, 0);
	XMStoreFloat2(&g, XMVector3Transform({}, m));
	sol2.x = atan2f(-g.y * position.x + g.x * position.y, g.x * position.x + g.y * position.y);

	if (sol1.x < 0)
		sol1.x += XM_2PI;
	if (sol2.x < 0)
		sol2.x += XM_2PI;

	return {
			(int)XMConvertToDegrees(sol1.x),
			(int)XMConvertToDegrees(sol1.y),
			(int)XMConvertToDegrees(sol2.x),
			(int)XMConvertToDegrees(sol2.y) };
}



bool KinematicChain::IsCollidingAny(DirectX::XMINT2 angles)
{
	if (!m_obstacles)
		return false;
	XMFLOAT2 point1 = ForwardKinematics(angles.x);
	XMFLOAT2 point2 = ForwardKinematics(angles);
	for (int i = 0; i < m_obstacles->size(); i++)
	{
		auto& a = (*m_obstacles)[i].a, &b = (*m_obstacles)[i].b;
		auto ab = XMFLOAT2{ a.x, b.y }, ba = XMFLOAT2{ b.x,a.y };

		if (
			vecmath::bb2d(a, b, {}) ||
			vecmath::segments2d(a, ab, XMFLOAT2{}, point1) ||
			vecmath::segments2d(a, ba, XMFLOAT2{}, point1) ||
			vecmath::segments2d(ab, b, XMFLOAT2{}, point1) ||
			vecmath::segments2d(ba, b, XMFLOAT2{}, point1) ||
			vecmath::segments2d(a, ab, point1, point2) ||
			vecmath::segments2d(a, ba, point1, point2) ||
			vecmath::segments2d(ab, b, point1, point2) ||
			vecmath::segments2d(ba, b, point1, point2)
			)
			return true;
	}
	return false;
}

void KinematicChain::FindPath()
{
	memset(m_configSpace, 0, sizeof(m_configSpace[0]) * c_configSpaceSize * c_configSpaceSize);
	m_foundStart = false;
	m_isConfigSpaceCalculated = true;
	std::queue<XMINT2> positions;
	positions.push(m_r_end);
	m_configSpace[m_r_end.x + m_r_end.y * c_configSpaceSize] = 1;
	while (!positions.empty())
	{
		XMINT2 p = positions.front();positions.pop();
		int16_t pv = m_configSpace[p.x + p.y * c_configSpaceSize];

		XMINT2 c[8];
		c[0] = XMINT2{ p.x + 0, p.y + 1 };
		c[1] = XMINT2{ p.x + 1, p.y + 0 };
		c[2] = XMINT2{ p.x + 0, p.y - 1 };
		c[3] = XMINT2{ p.x - 1, p.y + 0 };
		c[4+0] = XMINT2{ p.x + 1, p.y + 1 };
		c[4+1] = XMINT2{ p.x + 1, p.y - 1 };
		c[4+2] = XMINT2{ p.x - 1, p.y - 1 };
		c[4+3] = XMINT2{ p.x - 1, p.y + 1 };
		for (int i = 0; i < (m_isEuclidean ? 8 : 4); i++)
		{
			c[i].x += c_configSpaceSize; c[i].x %= c_configSpaceSize;
			c[i].y += c_configSpaceSize; c[i].y %= c_configSpaceSize;
			auto value = m_configSpace[c[i].x + c[i].y * c_configSpaceSize];
			if (value == 0 || value > pv + ((i < 4) ? 2 : 3))
			{
				if (IsCollidingAny(c[i]))
				{
					m_configSpace[c[i].x + c[i].y * c_configSpaceSize] = -1;
				}
				else if(c[i] == m_r_start)
				{
					m_foundStart = true;
				}
				else
				{
					m_steps = fmaxf(m_steps, pv + ((i<4)?2:3));
					m_configSpace[c[i].x + c[i].y * c_configSpaceSize] = pv + ((i < 4) ? 2 : 3);
					positions.push({ c[i].x, c[i].y });
				}
			}
		}
	}
}

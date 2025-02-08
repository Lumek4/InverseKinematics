#pragma once

#include"vecmath.h"
class KinematicChain
{
public:
	KinematicChain();
	void DrawConfigSpace();
	void Draw();
	void UpdateControls();
	void GUI();
	std::vector<AARect>* m_obstacles = nullptr;
	bool m_locked = true;
private:
	DirectX::XMFLOAT2 ForwardKinematics(float angle);
	DirectX::XMFLOAT2 ForwardKinematics(DirectX::XMINT2 angles);
	DirectX::XMINT4 InverseKinematics(DirectX::XMFLOAT2 position);
	DirectX::XMFLOAT2 m_lengths = { 0.5f, 0.5f };

	// picking
	DirectX::XMFLOAT2 m_p_effector{};
	DirectX::XMINT2 m_r_solution1 = {};
	DirectX::XMINT2 m_r_solution2 = {};
	bool m_viable1 = true, m_viable2 = true;

	// pathfinding
	bool IsCollidingAny(DirectX::XMINT2 angles);
	void FindPath();

	bool m_startSet = false;
	bool m_startFlippable = true;
	DirectX::XMFLOAT2 m_p_start{ 0.1, 0.8 };
	DirectX::XMINT2 m_r_start{};
	DirectX::XMINT2 m_r_start1{};
	DirectX::XMINT2 m_r_start2{};

	bool m_endSet = false;
	bool m_endFlippable = true;
	DirectX::XMFLOAT2 m_p_end{ -0.1, 0.8 };
	DirectX::XMINT2 m_r_end{};
	DirectX::XMINT2 m_r_end1{};
	DirectX::XMINT2 m_r_end2{};

	DirectX::XMINT2 m_r_animation = {};
	bool m_isAnimating = false;
	static const int c_animation_rate = 16;
	int m_animationSkip = 0;
	int m_steps = 1;
	bool m_foundStart = false;

	bool m_isEuclidean = false;

	inline uint32_t GetColor(int16_t value);
	static const size_t c_configSpaceSize = 360;
	int16_t m_configSpace[c_configSpaceSize * c_configSpaceSize];
	bool m_isConfigSpaceCalculated = false;
};
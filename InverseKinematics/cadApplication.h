#pragma once
#include"dxApplication.h"
#include<DirectXMath.h>
#include<thread>
#include"locked_variable.h"
#include"cyclic_buffer_interface.h"
#include"timer.h"
#include"blockerCanvas.h"
#include"kinematicChain.h"
#include<random>

class CadApplication : public DxApplication
{
public:
	explicit CadApplication(HINSTANCE hInstance);
	~CadApplication();
private:
	void OnResize() override;
private:
	void GUI() override;
	void Update() override;
	void Render() override;
	//void Draw();
private:
	BlockerCanvas m_blockerCanvas{};
	KinematicChain m_kinematicChain{};
	std::unique_ptr<cyclic_buffer_interface<double>> m_frameTimes;

	timer m_frameTimer;
};
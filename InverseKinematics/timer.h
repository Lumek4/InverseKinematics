#pragma once
#include<chrono>

class timer
{
private:
	double m_time;
	double m_dt;
public:
	timer();
	void lap();

	double getDeltaTime() { return m_dt; }
	double getTime() { return m_time; }
private:
	std::chrono::high_resolution_clock::time_point m_start;
};
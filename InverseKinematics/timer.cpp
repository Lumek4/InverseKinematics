#include "timer.h"

timer::timer()
{
	m_start = std::chrono::high_resolution_clock::now();
	m_dt = NAN;
	m_time = 0;
}

void timer::lap()
{
	double oldTime = m_time;
	m_time = (std::chrono::high_resolution_clock::now() - m_start).count() * 1e-9;
	m_dt = m_time - oldTime;
}

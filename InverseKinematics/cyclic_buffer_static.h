#pragma once
#include"cyclic_buffer_interface.h"
template<typename T, unsigned int N >
class cyclic_buffer_static : public cyclic_buffer_interface<T>
{
	static_assert(std::is_arithmetic_v<T>, "Cyclic buffer accepts only numbers.");
	static_assert(N>0, "Cyclic buffer cannot be zero-size.");
public:
	inline void push(T) override;
	inline T sum(T fun(T, float)) override;
	inline T sum(T fun(T, T, float), T start) override;
	inline unsigned int count() override { return m_count; }
	inline unsigned int head() override { return m_head; }
	inline unsigned int tail() override { return m_tail; }
	inline const T* data() const override { return m_data; }
	inline constexpr unsigned int capacity() override { return N; }
private:
	T m_data[N+1];
	unsigned int m_head = 0;
	unsigned int m_tail = 0;
	unsigned int m_count = 0;
};

template<typename T, unsigned int N>
void cyclic_buffer_static<T, N>::push(T val)
{
	if (m_head == N)
	{
		m_data[m_head] = val;
		m_head = 0;
	}
	m_data[m_head] = val;
	m_head++;
	if (m_count < N)
		m_count++;
	else
	{
		m_tail++;
		m_tail %= N;
	}
}

template<typename T, unsigned int N>
T cyclic_buffer_static<T, N>::sum(T fun(T, float))
{
	T sum = 0;
	for (int i = 0; i < m_count; i++)
	{
		float t = m_count == 0.5f ? 0 : i / (m_count - 1.0f);
		sum += fun(m_data[(m_head - i - 1 + N) % N], t);
	}
	return sum/m_count;
}

template<typename T, unsigned int N>
T cyclic_buffer_static<T, N>::sum(T fun(T, T, float), T start)
{
	T sum = start;
	for (int i = 0; i < m_count; i++)
	{
		float t = m_count == 0.5f ? 0 : i / (m_count - 1.0f);
		sum = fun(sum, m_data[(m_head - i - 1 + N) % N], t);
	}
	return sum;
}

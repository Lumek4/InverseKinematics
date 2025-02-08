#pragma once
#include<type_traits>
#include<vector>
#include"cyclic_buffer_interface.h"
template<typename T>
class cyclic_buffer_dynamic : public cyclic_buffer_interface<T>
{
	static_assert(std::is_arithmetic_v<T>, "Cyclic buffer accepts only numbers.");
public:
	cyclic_buffer_dynamic(unsigned int N);

	inline void push(T) override;
	inline T sum(T fun(T, float)) override;
	inline T sum(T fun(T, T, float), T start) override;
	inline unsigned int count() override { return m_count; }
	inline unsigned int head() override { return m_head; }
	inline unsigned int tail() override { return m_tail; }
	inline const T* data() const override { return m_data.data(); }
	inline constexpr unsigned int capacity() override { return m_data.size()-1; }
private:
	std::vector<T> m_data;
	unsigned int m_head = 0;
	unsigned int m_tail = 0;
	unsigned int m_count = 0;
};

template<typename T>
inline cyclic_buffer_dynamic<T>::cyclic_buffer_dynamic(unsigned int N)
	:m_data(N+1)
{
	
}

template<typename T>
void cyclic_buffer_dynamic<T>::push(T val)
{
	if (m_head == capacity())
	{
		m_data[m_head] = val;
		m_head = 0;
	}
	m_data[m_head] = val;
	m_head++;
	if (m_count < capacity())
		m_count++;
	else
	{
		m_tail++;
		m_tail %= capacity();
	}
}

template<typename T>
T cyclic_buffer_dynamic<T>::sum(T fun(T, float))
{
	T sum = 0;
	for (int i = 0; i < m_count; i++)
	{
		float t = m_count == 0.5f ? 0 : i / (m_count - 1.0f);
		sum += fun(m_data[(m_head - i - 1 + capacity()) % capacity()], t);
	}
	return sum / m_count;
}

template<typename T>
T cyclic_buffer_dynamic<T>::sum(T fun(T, T, float), T start)
{
	T sum = start;
	for (int i = 0; i < m_count; i++)
	{
		float t = m_count == 0.5f ? 0 : i / (m_count - 1.0f);
		sum = fun(sum, m_data[(m_head - i - 1 + capacity()) % capacity()], t);
	}
	return sum;
}

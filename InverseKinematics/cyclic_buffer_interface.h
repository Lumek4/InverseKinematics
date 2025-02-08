#pragma once

template<typename T>
class cyclic_buffer_interface
{
public:
	virtual void push(T) = 0;
	virtual T sum(T fun(T, float)) = 0;
	virtual T sum(T fun(T, T, float), T start) = 0;
	virtual unsigned int count() = 0;
	virtual unsigned int head() = 0;
	virtual unsigned int tail() = 0;
	virtual const T* data() const = 0;
	virtual unsigned int capacity() = 0;
};
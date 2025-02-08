#pragma once

#include<mutex>

template<typename T>
class locked_variable;

template<typename T>
class locked_reference
{
public:
	T& value;
private:
	locked_reference(locked_variable<T>& variable);
	std::lock_guard<std::mutex> m_lock;
	friend class locked_variable<T>;
};

template<typename T>
class locked_variable
{
public:
	locked_variable(T&& value);
public:
	locked_reference<T> get();
private:
	T m_value;
	std::mutex m_mtx;
	friend locked_reference<T>;
};


template<typename T>
inline locked_reference<T>::locked_reference(locked_variable<T>& variable)
	:m_lock(variable.m_mtx), value(variable.m_value)
{}


template<typename T>
inline locked_variable<T>::locked_variable(T&& value)
{
	m_value = std::move(value);
}

template<typename T>
inline locked_reference<T> locked_variable<T>::get()
{
	return locked_reference<T>(*this);
}
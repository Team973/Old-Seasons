//
//	Flag.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/26/10.
//

#include "Flag.hpp"

Flag::Flag()
{
	m_value = m_prev = false;
}

Flag::Flag(bool initialValue)
{
	m_value = m_prev = initialValue;
}

bool Flag::Get()
{
	return m_value;
}

void Flag::Set(bool v)
{
	m_value = v;
}

bool Flag::Triggered()
{
	return (m_value != m_prev);
}

bool Flag::TriggeredOn()
{
	return (m_value && Triggered());
}

bool Flag::TriggeredOff()
{
	return (!m_value && Triggered());
}

void Flag::ClearTrigger()
{
	m_prev = m_value;
}

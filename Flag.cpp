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

bool Flag::CheckTriggered()
{
	return (m_value != m_prev);
}

bool Flag::CheckTriggeredOn()
{
	return (m_value && CheckTriggered());
}

bool Flag::CheckTriggeredOff()
{
	return (!m_value && CheckTriggered());
}

bool Flag::GetTriggered()
{
	bool retval = CheckTriggered();
	ClearTrigger();
	return retval;
}

bool Flag::GetTriggeredOn()
{
	bool retval = CheckTriggeredOn();
	ClearTrigger();
	return retval;
}

bool Flag::GetTriggeredOff()
{
	bool retval = CheckTriggeredOff();
	ClearTrigger();
	return retval;
}

void Flag::ClearTrigger()
{
	m_prev = m_value;
}

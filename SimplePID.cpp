//
//	SimplePID.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/14/10.
//

#include "SimplePID.hpp"

SimplePID::SimplePID(double p, double i, double d)
{
	m_timer = NULL;
	SetPID(p, i, d);
	Reset();
}

SimplePID::~SimplePID()
{
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
	}
}

void SimplePID::SetPID(double p, double i, double d)
{
	m_P = p;
	m_I = i;
	m_D = d;
}

void SimplePID::Start()
{
	m_timer->Start();
	m_timer->Reset();
}

void SimplePID::Stop()
{
	m_timer->Stop();
}

void SimplePID::Reset()
{
	m_previousError = 0.0;
	m_integral = 0.0;
	m_target = 0.0;
	m_output = 0.0;
	
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
	}
	m_timer = new Timer();
}

double SimplePID::Update(double actual)
{
	double time = m_timer->Get();
	m_timer->Reset();
	return Update(actual, time);
}

double SimplePID::Update(double actual, double time)
{
	double error = m_target - actual;
	double derivative;
	
	if (time <= 0.0)
		time = 0.001;
	
	m_integral += error * time;
	derivative = (error - m_previousError) / time;
	m_output = (m_P * error) + (m_I * m_integral) + (m_D * derivative);
	m_previousError = error;
	
	if (m_hasMax && m_output > m_max)
		m_output = m_max;
	if (m_hasMin && m_output < m_min)
		m_output = m_min;
	
	return m_output;
}

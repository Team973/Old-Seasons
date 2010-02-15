//
//	SimplePID.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/14/10.
//

#include "WPILib.h"

#ifndef _BOSS_973_SIMPLEPID_H_
#define _BOSS_973_SIMPLEPID_H_

class SimplePID
{
protected:
	float m_P, m_I, m_D;
	float m_integral;
	double m_previousError;
	float m_target, m_output;
	
	float m_max, m_min;
	bool m_hasMax, m_hasMin;
	
	Timer *m_timer;
public:
	SimplePID(float p=0.0, float i=0.0, float d=0.0);
	~SimplePID();
	void Start();
	void Stop();
	void Reset();
	void SetPID(float p, float i, float d);
	
	inline float GetTarget(void)	{ return m_target; }
	inline void SetTarget(float t)	{ m_target = t; }

	inline void SetMin(float m)
	{
		m_hasMin = true;
		m_min = m;
	}
	inline void ClearMin()
	{
		m_hasMin = false;
	}
	inline void SetMax(float m)
	{
		m_hasMax = true;
		m_max = m;
	}
	inline void ClearMax()
	{
		m_hasMax = false;
	}
	inline void SetLimits(float minimum, float maximum)
	{
		SetMin(minimum);
		SetMax(maximum);
	}
	
	float Update(float actual, float time);
	float Update(float actual);
	inline float Update(PIDSource *source)
	{
		return Update(source->PIDGet());
	}
	
	inline float GetOutput(void)
	{
		return m_output;
	}
};

#endif

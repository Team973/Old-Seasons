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
	double m_P, m_I, m_D;
	double m_previousError;
	double m_integral;
	double m_target, m_output;
	
	double m_max, m_min;
	bool m_hasMax, m_hasMin;
	
	Timer *m_timer;
public:
	SimplePID(double p=0.0, double i=0.0, double d=0.0);
	~SimplePID();
	void Start();
	void Stop();
	void Reset();
	void SetPID(double p, double i, double d);
	
	inline double GetTarget(void)	{ return m_target; }
	inline void SetTarget(double t)	{ m_target = t; }

	inline void SetMin(double m)
	{
		m_hasMin = true;
		m_min = m;
	}
	inline void ClearMin()
	{
		m_hasMin = false;
	}
	inline void SetMax(double m)
	{
		m_hasMax = true;
		m_max = m;
	}
	inline void ClearMax()
	{
		m_hasMax = false;
	}
	inline void SetLimits(double minimum, double maximum)
	{
		SetMin(minimum);
		SetMax(maximum);
	}
	inline void ClearLimits()
	{
		ClearMin();
		ClearMax();
	}
	
	double Update(double actual, double time);
	double Update(double actual);
	inline double Update(PIDSource *source)
	{
		return Update(source->PIDGet());
	}
	
	inline double GetOutput(void)
	{
		return m_output;
	}
};

#endif

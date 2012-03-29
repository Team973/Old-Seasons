/**
 *	@file SimplePID.hpp
 *	Header for the SimplePID class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/14/10.
 */

#include "WPILib.h"

#ifndef _BOSS_973_SIMPLEPID_H_
#define _BOSS_973_SIMPLEPID_H_

/**
 *	A simple proportional–integral–derivative controller.
 *
 *	For a full explanation of PID control, see the Wikipedia article:
 *	http://en.wikipedia.org/wiki/PID_controller
 *
 *	There is a PIDController class in the WPILib, but it demands that it be in
 *	control of a motor at all times.  For some purposes, this is fine, but
 *	SimplePID allows you to choose when you are on PID and when you aren't.
 *
 *	For convenience, SimplePID can also run a timer for you so that you don't
 *	have to determine the amount of time elapsed between #Update(double, double)
 *	calls yourself.  This is entirely optional.
 */
class SimplePID
{
private:
	double m_P, m_I, m_D;
	double m_previousError;
	double m_integral;
	double m_target, m_output;
	
	double m_max, m_min;
	bool m_hasMax, m_hasMin;
	
	Timer *m_timer;
public:
	/**
	 *	Create a new PID controller.
	 *
	 *	@param p
	 *		The proportional constant in the controller
	 *	@param i
	 *		The integral gain constant in the controller
	 *	@param d
	 *		The derivative gain constant in the controller
	 */
	SimplePID(double p=0.0, double i=0.0, double d=0.0);
	
	~SimplePID();
	
	/** Start the PID timer */
	void Start();
	
	/** Stop the PID timer */
	void Stop();
	
	/**
	 *	Reset the PID controller.
	 *
	 *	This will wipe most all values from the PID controller, so use with
	 *	extreme caution.  The values affected are:
	 *		- the previous error sample
	 *		- the integral accumulator
	 *		- the target
	 *		- the output
	 *		- the timer
	 *
	 *	All of the values mentioned above except the timer will be set to zero.
	 *	The timer will be reset and stopped.
	 *
	 *	It is important to note that the PID constants will not be altered by
	 *	this method.
	 */
	void Reset();
	
	/**
	 *	Change the PID constants.
	 *
	 *	It is highly recommended that you call #Reset immediately after this
	 *	method.  Strange things may happen otherwise.
	 *
	 *	@param p
	 *		The proportional constant in the controller
	 *	@param i
	 *		The integral gain constant in the controller
	 *	@param d
	 *		The derivative gain constant in the controller
	 */
	void SetPID(double p, double i, double d);
	
	/**
	 *	Obtain the current target.
	 *
	 *	@return The current target
	 */
	inline double GetTarget(void)	{ return m_target; }
	
	/**
	 *	Change the current target.
	 *
	 *	@param t	The new target
	 */
	inline void SetTarget(double t)	{ m_target = t; }
	
	/**
	 *	Set the minimum output.
	 *
	 *	When a minimum bound is set, the output will never be below this value.
	 *	By default, PID controllers do not have a minimum.
	 *
	 *	@param m
	 *		The new minimum
	 *	@see SetLimits
	 */
	inline void SetMin(double m)
	{
		m_hasMin = true;
		m_min = m;
	}
	
	/** Remove the minimum bound on output. */
	inline void ClearMin()
	{
		m_hasMin = false;
	}
	
	/**
	 *	Set the maximum output.
	 *
	 *	When a maximum bound is set, the output will never be above this value.
	 *	By default, PID controllers do not have a maximum.
	 *
	 *	@param m
	 *		The new maximum
	 *	@see SetLimits
	 */
	inline void SetMax(double m)
	{
		m_hasMax = true;
		m_max = m;
	}
	
	/** Remove the maximum bound on output. */
	inline void ClearMax()
	{
		m_hasMax = false;
	}
	
	/**
	 *	Set the minimum and maximum output.
	 *
	 *	When bounds are set on a PID controller, the output will always be
	 *	within the range given by the minimum and maximum.  By default, PID
	 *	controllers do not have either a minimum nor a maximum.
	 *
	 *	@param minimum
	 *		The new minimum limit for the output
	 *	@param maximum
	 *		The new maximum limit for the output
	 *	@see SetMin, SetMax, ClearLimits
	 */
	inline void SetLimits(double minimum, double maximum)
	{
		SetMin(minimum);
		SetMax(maximum);
	}
	
	/**
	 *	Remove the output's minimum and maximum bounds.
	 *
	 *	Without bounds on a PID controller, there is no guarantee on how large
	 *	or how small the output is.  By default, PID controllers do not have
	 *	either a minimum nor a maximum.
	 *
	 *	@see ClearMin, ClearMax, SetLimits
	 */
	inline void ClearLimits()
	{
		ClearMin();
		ClearMax();
	}
	
	/**
	 *	Update the PID controller.
	 *
	 *	The computation involves finding the difference between the input and
	 *	the target, the difference from last time, and the integral of the
	 *	difference.
	 *
	 *	If a minimum or maximum was set on the PID controller (via #SetLimits or
	 *	any related method), then any output calculated will be clipped into the
	 *	range given.
	 *
	 *	@param actual
	 *		The input value to compute with
	 *	@param time
	 *		The amount of time that has elapsed since the previous call to
	 *		#Update(double, double).
	 *	@return The new output value
	 */
	double Update(double actual, double time);
	
	/**
	 *	Update the PID controller with the timer.
	 *
	 *	This just calls #Update(double, double) with the time given by the
	 *	internal timer.
	 *
	 *	<strong>YOU MUST CALL #Start BEFORE YOU CALL THIS METHOD!</strong>
	 *
	 *	@param actual
	 *		The input value to use
	 *	@return The new output value
	 *	@see Update(double, double)
	 */
	double Update(double actual);
	
	/**
	 *	Update the PID controller from a source.
	 *
	 *	This mainly exists to ease transition from code using WPILib's PID
	 *	control system to our class.
	 *
	 *	@param source
	 *		The input to acquire from
	 *	@return The new output value
	 *	@see Update(double, double)
	 */
	inline double Update(PIDSource *source)
	{
		return Update(source->PIDGet());
	}
	
	/**
	 *	Obtain the output from the PID controller.
	 *
	 *	@return The last value returned by #Update(double, double)
	 */
	inline double GetOutput(void)
	{
		return m_output;
	}
};

#endif

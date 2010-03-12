/**
 *	@file AbsoluteEncoder.hpp
 *	Header for the AbsoluteEncoder class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/12/10.
 */

#include "WPILib.h"

#ifndef _BOSS_973_ABSOLUTEENCODER_H_
#define _BOSS_973_ABSOLUTEENCODER_H_

/**
 *	An absolute encoder connected to an analog input.
 *
 *	This absolute encoder class has functionality to use the encoder as though
 *	it is an incremental encoder, so when the encoder is turned and it wraps
 *	around, then this class will yield a value greater than the maximum voltage.
 */
class AbsoluteEncoder
{
private:
	float m_voltage, m_accumulator;
	float m_maxVoltage;
	AnalogChannel *m_channel;
	bool m_createdChannel;
public:
	explicit AbsoluteEncoder(UINT32 channel, float maxVoltage=5.0);
	AbsoluteEncoder(UINT32 slot, UINT32 channel, float maxVoltage=5.0);
	explicit AbsoluteEncoder(AnalogChannel *channel, float maxVoltage=5.0);
	~AbsoluteEncoder();
	
	/**
	 *	Get the current voltage of the encoder.
	 *
	 *	This is the absolute position of the encoder, not the calculated
	 *	incremental value.
	 *
	 *	@return The current voltage of the encoder
	 */
	float GetVoltage();
	
	/**
	 *	Get the current incremental voltage of the encoder.
	 *
	 *	This can give back negative values or values greater than the maximum
	 *	voltage.
	 *
	 *	@return The incremental voltage
	 */
	float GetIncrementalVoltage();
	
	/**
	 *	Reset the accumulator.
	 *
	 *	This does not make the current value zero.  2.1V will always be the same
	 *	position as 2.1V.
	 */
	void ResetAccumulator();
	
	/**
	 *	Update the accumulator.
	 *
	 *	This gets called automatically by #GetVoltage and
	 *	#GetIncrementalVoltage.
	 */
	void Update();
private:
	void InitEncoder(float maxVoltage);
};

#endif

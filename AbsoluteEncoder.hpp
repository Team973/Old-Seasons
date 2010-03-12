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
	
	float Get();
	float GetVoltage();
	void ResetAccumulator();
private:
	void InitEncoder(float maxVoltage);
	void Update();
};

#endif

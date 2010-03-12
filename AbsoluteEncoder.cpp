/**
 *	@file AbsoluteEncoder.cpp
 *	Implementation of the AbsoluteEncoder class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/12/10.
 */

AbsoluteEncoder::AbsoluteEncoder(UINT32 channel, float maxVoltage)
{
	m_channel = new AnalogChannel(channel);
	m_createdChannel = true;
	InitEncoder(maxVoltage);
}

AbsoluteEncoder::AbsoluteEncoder(UINT32 slot, UINT32 channel, float maxVoltage)
{
	m_channel = new AnalogChannel(slot, channel);
	m_createdChannel = true;
	InitEncoder(maxVoltage);
}

AbsoluteEncoder::AbsoluteEncoder(AnalogChannel *channel, float maxVoltage)
{
	m_channel = channel;
	m_createdChannel = false;
	InitEncoder(maxVoltage);
}

void AbsoluteEncoder::InitEncoder(float maxVoltage)
{
	m_accumulator = 0.0;
	m_voltage = 0.0;
	m_maxVoltage = maxVoltage;
}

AbsoluteEncoder::~AbsoluteEncoder()
{
	if (m_createdChannel)
	{
		delete m_channel;
	}
}

float AbsoluteEncoder::GetVoltage()
{
	Update();
	return m_voltage;
}

float AbsoluteEncoder::GetIncrementalVoltage()
{
	Update();
	return m_accumulator + m_voltage;
}

void AbsoluteEncoder::ResetAccumulator()
{
	m_accumulator = 0.0;
}

void AbsoluteEncoder::Update()
{
	float newVoltage = m_channel->GetVoltage();
	
	if (fabsf(newVoltage - m_voltage) > (m_maxVoltage / 2))
	{
		if (newVoltage > m_voltage)
		{
			// The new voltage has wrapped back to the highest voltage
			m_accumulator -= m_maxVoltage;
		}
		else
		{
			// The new voltage has wrapped around to the lowest voltage
			m_accumulator += m_maxVoltage;
		}
	}
	
	m_voltage = newVoltage;
}

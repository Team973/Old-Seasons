// ControlBoard.cpp
// Created on 2/11/10.

#include "ControlBoard.hpp"

ControlBoard::ControlBoard()
	: m_stick1(1), m_stick2(2), m_stick3(3)
{
}

Joystick &ControlBoard::GetJoystick(int index)
{
	switch (index)
	{
	case 1:
		return m_stick1;
	case 2:
		return m_stick2;
	case 3:
		return m_stick3;
	default:
		// TODO: Error
		return m_stick1;
	}
}

/**
 * Get the state of a given button on the driver station.
 * 
 * Why don't we just query it directly?
 * General code friendliness, and some of the buttons are wired bizarrely, so we
 * invert the ones we need to here.
 */
bool ControlBoard::GetButton(UINT16 buttonNum)
{
	bool value = DriverStation::GetInstance()->GetDigitalIn(buttonNum);
	bool invert = true;
	// TODO: Find buttons to not invert
	if (invert)
		value = !value;
	return value;
}

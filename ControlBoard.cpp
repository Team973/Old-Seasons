//
//	ControlBoard.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "ControlBoard.hpp"

static ControlBoard *gControlBoardInstance = NULL;

ControlBoard::ControlBoard()
	: m_stick1(1), m_stick2(2), m_stick3(3)
{
#ifdef FEATURE_IO_BOARD
	DriverStation::GetInstance()->GetEnhancedIO().GetDigitalConfig(1);
#endif
}

ControlBoard &ControlBoard::GetInstance()
{
    if (gControlBoardInstance == NULL)
    {
        gControlBoardInstance = new ControlBoard();
    }
    return *gControlBoardInstance;
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

bool ControlBoard::GetButton(UINT16 buttonNum)
{
#ifdef FEATURE_IO_BOARD
	bool value = DriverStation::GetInstance()->GetEnhancedIO().GetDigital(buttonNum);
	bool invert = false;
	if (invert)
		value = !value;
	return value;
#else
	return false;
#endif
}

void ControlBoard::SetMultiLight(UINT16 lightNum, short state)
{
	SetMultiLight(lightNum, lightNum + 2, state);
}

void ControlBoard::SetMultiLight(UINT16 greenLight, UINT16 redLight, short state)
{
#ifdef FEATURE_IO_BOARD
	DriverStation::GetInstance()->GetEnhancedIO().SetDigitalOutput(redLight, state & kLightRed);
	DriverStation::GetInstance()->GetEnhancedIO().SetDigitalOutput(greenLight, state & kLightGreen);
#endif
}

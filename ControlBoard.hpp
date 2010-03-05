//
//	ControlBoard.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"
#include "Options.hpp"

#ifndef _BOSS_973_CONTROLBOARD_H_
#define _BOSS_973_CONTROLBOARD_H_

class ControlBoard
{
protected:
	Joystick m_stick1, m_stick2, m_stick3;
	
	ControlBoard();
public:
	enum
	{
		kLightOff = 0,
		kLightRed = 1,
		kLightGreen = 2,
		kLightYellow = 3,
	};
	
    static ControlBoard &GetInstance();
    
	Joystick &GetJoystick(int index);
	bool GetButton(UINT16 buttonNum);
	void SetMultiLight(UINT16 lightNum, short state);
	void SetMultiLight(UINT16 greenLight, UINT16 redLight, short state);
};

#endif

//
//	ControlBoard.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"

#ifndef _BOSS_973_CONTROLBOARD_H_
#define _BOSS_973_CONTROLBOARD_H_

class ControlBoard
{
protected:
	Joystick m_stick1, m_stick2, m_stick3;
	
	ControlBoard();
public:
    static ControlBoard &GetInstance();
    
	Joystick &GetJoystick(int index);
	bool GetButton(UINT16 buttonNum);
};

#endif

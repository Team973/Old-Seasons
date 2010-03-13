/**
 *	@file DisabledState.cpp
 *	Implementation of the DisabledState class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/12/10.
 */

#include "State.hpp"
#include "ControlBoard.hpp"

DisabledState::DisabledState(BossRobot *r, State *s) : State(r)
{
	m_prevState = s;
}

void DisabledState::Enter()
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->Clear();
	lcd->PrintfLine(DS_LCD::kUser_Line1, "Soft-disabled");
	if (m_prevState != NULL)
	{
		lcd->PrintfLine(DS_LCD::kUser_Line3, "Flip disable switch");
		lcd->PrintfLine(DS_LCD::kUser_Line4, "to regain control");
	}
	lcd->UpdateLCD();
#endif
	
	m_robot->SoftStop();
}

void DisabledState::Exit()
{
}

void DisabledState::Step()
{
	m_switch.Set(ControlBoard::GetInstance().GetButton(2));
	if (m_prevState != NULL && m_switch.GetTriggeredOff())
	{
		// We flipped the switch off.  Go back to your business.
		m_robot->ChangeState(m_prevState);
		return;
	}
}

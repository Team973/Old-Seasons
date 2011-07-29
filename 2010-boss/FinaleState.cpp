/**
 *	@file FinaleState.cpp
 *	Implementation of the FinaleState class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/9/10.
 */

#include "State.hpp"
#include "DriveSystem.hpp"
#include "ArmSystem.hpp"
#include "ControlBoard.hpp"
#include "NormalState.hpp"

FinaleState::FinaleState(BossRobot *r) : State(r)
{
}

void FinaleState::Enter()
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->Clear();
	lcd->PrintfLine(DS_LCD::kUser_Line1, "FINALE!!!!!");
	lcd->UpdateLCD();
#endif
	
	m_robot->SoftStop();
	
	m_robot->GetArmSystem()->SetState(ArmSystem::kRaised);
}

void FinaleState::Exit()
{
}

void FinaleState::Step()
{
	TeleoperatedDriveSystem *ds = dynamic_cast<TeleoperatedDriveSystem *>(m_robot->GetDriveSystem());
	
	// Check for state finished
	if (ControlBoard::GetInstance().GetButton(2))
	{
		m_robot->ChangeState(new DisabledState(m_robot, this));
		return;
	}
	else if (!ControlBoard::GetInstance().GetButton(3))
	{
		m_robot->ChangeState(new NormalState(m_robot));
		return;
	}
	else if (ControlBoard::GetInstance().GetJoystick(3).GetTrigger())
	{
		m_robot->ChangeState(new RaisingState(m_robot));
		return;
	}
	
	if (ds != NULL)
	{
		ds->ReadControls();
		ds->Compensate();
		m_robot->GetWatchdog().Feed();
		ds->Drive();
	}
	m_robot->GetWatchdog().Feed();
	
	m_robot->GetArmSystem()->Update();
}

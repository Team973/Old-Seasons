/**
 *	@file NormalState.cpp
 *	Implementation of the NormalState class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/12/10.
 */

#include "NormalState.hpp"
#include "ConfigState.hpp"

#include "ControlBoard.hpp"
#include "DriveSystem.hpp"
#include "KickerSystem.hpp"

NormalState::NormalState(BossRobot *r)
	: State(r)
{
}

void NormalState::Enter()
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->PrintfLine(DS_LCD::kUser_Line1, "Normal operation");
#endif
	
	// Initialize drive system
	std::string driveSystemName = m_robot->GetConfig().SetDefault("driveSystem", "arcade");
	if (driveSystemName == "arcade")
	{
		m_robot->SetDriveSystem(new ArcadeDriveSystem(m_robot, m_robot->GetDrive()));
	}
	else if (driveSystemName == "tank")
	{
		m_robot->SetDriveSystem(new TankDriveSystem(m_robot, m_robot->GetDrive()));
	}
	else if (driveSystemName == "xbox")
	{
		m_robot->SetDriveSystem(new XboxDriveSystem(m_robot, m_robot->GetDrive()));
	}
	else
	{
		// Default to arcade system
		m_robot->SetDriveSystem(new ArcadeDriveSystem(m_robot, m_robot->GetDrive()));
	}
	
#ifdef FEATURE_DRIVE_ENCODERS
	m_robot->GetLeftDriveEncoder()->Start();
	m_robot->GetRightDriveEncoder()->Start();
	m_robot->GetLeftDriveEncoder()->Reset();
	m_robot->GetRightDriveEncoder()->Reset();
#endif
}

void NormalState::Exit()
{
#ifdef FEATURE_DRIVE_ENCODERS
	m_robot->GetLeftDriveEncoder()->Reset();
	m_robot->GetRightDriveEncoder()->Reset();
#endif
}

void NormalState::Step()
{
	TeleoperatedDriveSystem *ds = dynamic_cast<TeleoperatedDriveSystem *>(m_robot->GetDriveSystem());
	
	if (ControlBoard::GetInstance().GetButton(1))
	{
		m_robot->ChangeState(new ConfigState(m_robot));
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
	
	if (m_robot->GetKickerSystem() != NULL)
	{
		m_robot->GetKickerSystem()->ReadControls();
		m_robot->GetKickerSystem()->Update();
	}
	
	m_robot->GetWatchdog().Feed();
}

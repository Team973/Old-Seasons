//
//	NormalState.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/12/10.
//

#include "NormalState.hpp"

#include "DriveSystem.hpp"

NormalState::NormalState(BossRobot *r)
	: State(r)
{
	mKicker = new KickerSystem();
}

void NormalState::Enter()
{
	m_robot->SetDriveSystem(new ArcadeDriveSystem(m_robot, m_robot->GetDrive()));
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
	
	if (ds != NULL)
	{
		ds->ReadControls();
		ds->Compensate();
		m_robot->GetWatchdog().Feed();
		ds->Drive();
	}
	m_robot->GetWatchdog().Feed();
	
#ifdef FEATURE_UPPER_BOARD
	mKicker->ReadControls();
	mKicker->Update();
#endif
	
	m_robot->GetWatchdog().Feed();
}

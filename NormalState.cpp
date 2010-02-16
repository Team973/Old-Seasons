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
}

void NormalState::Enter()
{
	m_robot->SetDriveSystem(new ArcadeDriveSystem(m_robot, m_robot->GetDrive()));
	m_robot->GetLeftDriveEncoder()->Start();
	m_robot->GetRightDriveEncoder()->Start();
	m_robot->GetLeftDriveEncoder()->Reset();
	m_robot->GetRightDriveEncoder()->Reset();
}

void NormalState::Exit()
{
	m_robot->GetLeftDriveEncoder()->Reset();
	m_robot->GetRightDriveEncoder()->Reset();
}

void NormalState::Step()
{
	TeleoperatedDriveSystem *ds = dynamic_cast<TeleoperatedDriveSystem *>(m_robot->GetDriveSystem());
	
	ds->ReadControls();
	ds->Compensate();
	ds->Drive();
}

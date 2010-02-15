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
	m_still = m_prevStill = false;
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
	ds->Drive();
	
	m_still = !ds->IsMoving();
	
	if (m_still && m_still != m_prevStill)
	{
		m_robot->GetLeftDriveEncoder()->Reset();
		m_robot->GetRightDriveEncoder()->Reset();
	}
	
	m_prevStill = m_still;
}

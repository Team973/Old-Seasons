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
}

void NormalState::Exit()
{
}

void NormalState::Step()
{
	m_robot->GetDriveSystem()->ReadControls();
	m_robot->GetDriveSystem()->Drive();
}

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
	
	if (m_still)
	{
		if (m_still != m_prevStill)
		{
			m_robot->GetLeftDriveEncoder()->Reset();
			m_robot->GetRightDriveEncoder()->Reset();
			
			m_leftPID.SetPID(0.05, 0.0, 0.0);
			m_leftPID.Reset();
			m_leftPID.SetTarget(0.0);
			m_leftPID.SetLimits(-1.0, 1.0);
			m_leftPID.Start();

			m_rightPID.SetPID(0.05, 0.0, 0.0);
			m_rightPID.Reset();
			m_rightPID.SetTarget(0.0);
			m_rightPID.SetLimits(-1.0, 1.0);
			m_rightPID.Start();
		}
		else
		{
			double leftSpeed = 0.0, rightSpeed = 0.0;
			
			m_leftPID.Update(m_robot->GetLeftDriveEncoder()->Get());
			m_rightPID.Update(m_robot->GetRightDriveEncoder()->Get());
			
			if (abs(m_robot->GetLeftDriveEncoder()->Get()))
			{
				leftSpeed = m_leftPID.GetOutput();
			}
			
			if (abs(m_robot->GetRightDriveEncoder()->Get()))
			{
				rightSpeed = m_rightPID.GetOutput();
			}
			
			ds->GetDrive()->SetLeftRightMotorSpeeds(leftSpeed, rightSpeed);
		}
	}
	
	m_prevStill = m_still;
}

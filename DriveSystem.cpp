//
//	DriveSystem.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "DriveSystem.hpp"
#include "ControlBoard.hpp"
#include "MyDriverStationLCD.h"

static inline float limit(float n)
{
	if (n > 1.0)
		return 1.0;
	else if (n < -1.0)
		return -1.0;
	else
		return n;
}

static inline float fmax(float a, float b)
{
	return (a >= b) ? a : b;
}

static inline float sign_square(float n)
{
	return (n >= 0.0) ? (n * n) : -(n * n);
}

DriveSystem::DriveSystem()
{
	m_robot = NULL;
	m_drive = NULL;
	m_leftSpeed = m_rightSpeed = 0.0;
	m_prevMoving = false;
	m_firstMoveComp = true;
	m_deadheadPID.SetLimits(-0.25, 0.25);
}

DriveSystem::DriveSystem(BossRobot *r, RobotDrive *d)
{
    m_robot = r;
	m_drive = d;
	m_leftSpeed = m_rightSpeed = 0.0;
	m_prevMoving = false;
	m_firstMoveComp = true;
	m_deadheadPID.SetLimits(-0.25, 0.25);
}

void DriveSystem::Drive()
{
	m_drive->SetLeftRightMotorSpeeds(m_leftSpeed, m_rightSpeed);
}

void DriveSystem::Stop()
{
	m_leftSpeed = m_rightSpeed = 0.0;
}

void DriveSystem::Compensate()
{
	bool newMoving = IsMoving();
	
	if (newMoving)
	{
		InertCompensate();
		m_firstMoveComp = true;
	}
	else
	{
		MovingCompensate();
	}
	
	m_prevMoving = newMoving;
}

#define INERT_P 0.05
#define INERT_I 0.0
#define INERT_D 0.0

void DriveSystem::InertCompensate()
{
	if (IsMoving() != m_prevMoving)
	{
		m_robot->GetLeftDriveEncoder()->Reset();
		m_robot->GetRightDriveEncoder()->Reset();
		
		m_leftPID.SetPID(INERT_P, INERT_I, INERT_D);
		m_leftPID.Reset();
		m_leftPID.SetTarget(0.0);
		m_leftPID.SetLimits(-1.0, 1.0);
		m_leftPID.Start();

		m_rightPID.SetPID(INERT_P, INERT_I, INERT_D);
		m_rightPID.Reset();
		m_rightPID.SetTarget(0.0);
		m_rightPID.SetLimits(-1.0, 1.0);
		m_rightPID.Start();
	}
	else
	{
		m_leftPID.Update(m_robot->GetLeftDriveEncoder()->Get());
		m_rightPID.Update(m_robot->GetRightDriveEncoder()->Get());
		
		if (abs(m_robot->GetLeftDriveEncoder()->Get()) < 15)
			m_leftSpeed = m_leftPID.GetOutput();
		if (abs(m_robot->GetRightDriveEncoder()->Get()) < 15)
			m_rightSpeed = m_rightPID.GetOutput();
	}
}

#undef INERT_P
#undef INERT_I
#undef INERT_D

#define MOVING_P 0.0005
#define MOVING_I 0.0
#define MOVING_D 0.0

void DriveSystem::MovingCompensate()
{
	if (IsTurning())
		return;
	
	if (m_firstMoveComp)
	{
		m_deadheadPID.SetPID(MOVING_P, MOVING_I, MOVING_D);
		m_deadheadPID.Reset();
		m_deadheadPID.SetTarget(m_robot->GetGyro()->GetAngle());
		m_deadheadPID.SetLimits(-0.25, 0.25);
		m_deadheadPID.Start();
	}
	else
	{
		m_deadheadPID.Update(m_robot->GetGyro()->GetAngle());
		
		m_leftSpeed = limit(m_leftSpeed - m_deadheadPID.GetOutput());
		m_rightSpeed = limit(m_rightSpeed + m_deadheadPID.GetOutput());
	}
	
	m_firstMoveComp = false;
}

#undef MOVING_P
#undef MOVING_I
#undef MOVING_D

bool DriveSystem::IsMoving()
{
	return (m_leftSpeed > 0.25 || m_leftSpeed < -0.25 ||
			m_rightSpeed > 0.25 || m_rightSpeed < -0.25);
}

bool DriveSystem::IsTurning()
{
	double delta = m_leftSpeed - m_rightSpeed;
	return (delta > 0.15 || delta < -0.15);
}

AutonomousDriveSystem::AutonomousDriveSystem(BossRobot *r, RobotDrive *d)
    : DriveSystem(r, d)
{
}
	
void AutonomousDriveSystem::ReadControls()
{	
}

ArcadeDriveSystem::ArcadeDriveSystem(BossRobot *r, RobotDrive *d)
    : TeleoperatedDriveSystem(r, d)
{
	m_move = m_rotate = 0.0;
}

void ArcadeDriveSystem::ReadControls()
{
	m_move = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_rotate = -(ControlBoard::GetInstance().GetJoystick(2).GetX());
	
	InterpretControls();
}

void ArcadeDriveSystem::InterpretControls()
{
	m_move = limit(m_move);
	m_rotate = limit(m_rotate);
	
	// Copied from WPILib
	if (1) // squaredInputs
	{
		m_move = sign_square(m_move);
		m_rotate = sign_square(m_rotate);
	}
	
	if (m_move > 0.0)
	{
		if (m_rotate > 0.0)
		{
			m_leftSpeed = m_move - m_rotate;
			m_rightSpeed = fmax(m_move, m_rotate);
		}
		else
		{
			m_leftSpeed = fmax(m_move, -m_rotate);
			m_rightSpeed = m_move + m_rotate;
		}
	}
	else
	{
		if (m_rotate > 0.0)
		{
			m_leftSpeed = -fmax(-m_move, m_rotate);
			m_rightSpeed = m_move + m_rotate;
		}
		else
		{
			m_leftSpeed = m_move - m_rotate;
			m_rightSpeed = -fmax(-m_move, -m_rotate);
		}
	}
}

bool ArcadeDriveSystem::IsMoving()
{
	return (m_move > 0.25 || m_move < -0.25 ||
			m_rotate > 0.25 || m_rotate < -0.25);
}

bool ArcadeDriveSystem::IsTurning()
{
	return (m_rotate > 0.15 || m_rotate < -0.15);
}

/* 	Xbox controller info
 * 
 * 	Axes (1-based):
 * 		1 - Left X
 * 		2 - Left Y
 * 		3 - Both triggers
 * 		4 - Right X
 * 		5 - Right Y
 */
XboxDriveSystem::XboxDriveSystem(BossRobot *r, RobotDrive *d)
    : ArcadeDriveSystem(r, d)
{
}

void XboxDriveSystem::ReadControls(void)
{
	m_move = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_rotate = -(ControlBoard::GetInstance().GetJoystick(1).GetRawAxis(4));
	
	InterpretControls();
}

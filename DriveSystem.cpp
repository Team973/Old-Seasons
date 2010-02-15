//
//	DriveSystem.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "DriveSystem.hpp"
#include "ControlBoard.hpp"

DriveSystem::DriveSystem()
{
	m_drive = NULL;
	m_leftSpeed = m_rightSpeed = 0.0;
	m_prevMoving = false;
}

DriveSystem::DriveSystem(BossRobot *r, RobotDrive *d)
{
    m_robot = r;
	m_drive = d;
	m_leftSpeed = m_rightSpeed = 0.0;
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
	if (!IsMoving())
	{
		InertCompensate();
	}
	else
	{
		MovingCompensate();
	}
	
	m_prevMoving = IsMoving();
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
		
		m_leftSpeed = m_leftPID.GetOutput();
		m_rightSpeed = m_rightPID.GetOutput();
	}
}

#undef INERT_P
#undef INERT_I
#undef INERT_D

void DriveSystem::MovingCompensate()
{
}

bool DriveSystem::IsMoving()
{
	return (m_leftSpeed > 0.25 || m_leftSpeed < -0.25 ||
			m_rightSpeed > 0.25 || m_rightSpeed < -0.25);
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

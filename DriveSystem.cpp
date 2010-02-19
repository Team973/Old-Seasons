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
	InitPID();
}

DriveSystem::DriveSystem(BossRobot *r, RobotDrive *d)
{
    m_robot = r;
	m_drive = d;
	m_leftSpeed = m_rightSpeed = 0.0;
	InitPID();
}

void DriveSystem::InitPID()
{
	double inertP = m_robot->GetConfig().SetDefault("inertP", 0.05);
	double inertI = m_robot->GetConfig().SetDefault("inertI", 0.0);
	double inertD = m_robot->GetConfig().SetDefault("inertD", 0.0);
	double movingP = m_robot->GetConfig().SetDefault("movingP", 5.e-3);
	double movingI = m_robot->GetConfig().SetDefault("movingI", 0.0);
	double movingD = m_robot->GetConfig().SetDefault("movingD", 0.0);
	
	m_prevMoving = false;
	m_firstMoveComp = true;
	
	m_leftPID.SetPID(inertP, inertI, inertD);
	m_leftPID.SetLimits(-1.0, 1.0);
	
	m_rightPID.SetPID(inertP, inertI, inertD);
	m_rightPID.SetLimits(-1.0, 1.0);
	
	m_deadheadPID.SetPID(movingP, movingI, movingD);
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

void DriveSystem::Compensate()
{
	bool newMoving = IsMoving();
	
	if (!newMoving)
	{
#ifdef FEATURE_DRIVE_ENCODERS
		InertCompensate();
#endif
		m_firstMoveComp = true;
	}
	else
	{
#ifdef FEATURE_GYRO
		MovingCompensate();
#endif
	}
	
	m_prevMoving = newMoving;
	
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->UpdateLCD();
#endif
}

void DriveSystem::InertCompensate()
{
	if (IsMoving() != m_prevMoving)
	{
		m_robot->GetLeftDriveEncoder()->Reset();
		m_robot->GetRightDriveEncoder()->Reset();
		
		m_leftPID.Reset();
		m_leftPID.SetTarget(0.0);
		m_leftPID.Start();
		
		m_rightPID.Reset();
		m_rightPID.SetTarget(0.0);
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

#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->PrintfLine(DS_LCD::kUser_Line2, "PID Inert");
#endif
}

void DriveSystem::MovingCompensate()
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
#endif
	
	if (IsTurning())
	{
#ifdef FEATURE_LCD
		lcd->PrintfLine(DS_LCD::kUser_Line2, "PID Off");
#endif
		return;
	}
	
	if (m_firstMoveComp)
	{
		m_deadheadPID.Reset();
		m_deadheadPID.SetTarget(m_robot->GetGyro()->GetAngle());
		m_deadheadPID.Start();
	}
	else
	{
		m_deadheadPID.Update(m_robot->GetGyro()->GetAngle());
		
		m_leftSpeed = limit(m_leftSpeed + m_deadheadPID.GetOutput());
		m_rightSpeed = limit(m_rightSpeed - m_deadheadPID.GetOutput());
		
#ifdef FEATURE_LCD
		lcd->PrintfLine(DS_LCD::kUser_Line2, "PID: %.4f", m_deadheadPID.GetOutput());
#endif
	}
	
	m_firstMoveComp = false;
}

/**** AUTONOMOUS ****/

AutonomousDriveSystem::AutonomousDriveSystem(BossRobot *r, RobotDrive *d)
    : DriveSystem(r, d)
{
}
	
void AutonomousDriveSystem::ReadControls()
{	
}

/**** ARCADE ****/

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
	return (m_move > 0.25 || m_move < -0.25 || IsTurning());
}

bool ArcadeDriveSystem::IsTurning()
{
	return (m_rotate > 0.15 || m_rotate < -0.15);
}

/**** XBOX ****/

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

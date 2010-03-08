/**
 *	@file DriveSystem.cpp
 *	Implementation of all of the different drive systems.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "DriveSystem.hpp"
#include "ControlBoard.hpp"
#include "MyDriverStationLCD.h"

/**
 *	Ensure that a number is within [-1.0, 1.0].
 *
 *	@param n
 *		The number to operate on
 *	@return The clipped number
 */
static inline float limit(float n)
{
	if (n > 1.0)
		return 1.0;
	else if (n < -1.0)
		return -1.0;
	else
		return n;
}

/**
 *	Return the maximum of two floating-point numbers.
 *
 *	@param a
 *		The first floating-point number
 *	@param b
 *		The second floating-point number
 *	@return The greater number of the two.
 */
static inline float fmax(float a, float b)
{
	return (a >= b) ? a : b;
}

/**
 *	Square a number, but keep the sign.
 *
 *	@param n
 *		The number to square
 *	@return The number squared with the same sign as the original.
 */
static inline float sign_square(float n)
{
	return (n >= 0.0) ? (n * n) : -(n * n);
}

DriveSystem::DriveSystem()
{
	m_robot = NULL;
	m_gear = kLoGear;
	m_leftSpeed = m_rightSpeed = 0.0;
	InitPID();
}

DriveSystem::DriveSystem(BossRobot *r)
{
    m_robot = r;
	m_gear = kLoGear;
	m_leftSpeed = m_rightSpeed = 0.0;
	InitPID();
}

void DriveSystem::InitPID()
{
	double inertP = m_robot->GetConfig().SetDefault("inertP", 0.01);
	double inertI = m_robot->GetConfig().SetDefault("inertI", 0.0);
	double inertD = m_robot->GetConfig().SetDefault("inertD", 0.0);
	double movingP = m_robot->GetConfig().SetDefault("movingP", 5.e-3);
	double movingI = m_robot->GetConfig().SetDefault("movingI", 0.0);
	double movingD = m_robot->GetConfig().SetDefault("movingD", 0.0);
	
	m_firstInertComp = m_firstMovingComp = true;
	
	m_leftPID.SetPID(inertP, inertI, inertD);
	m_leftPID.SetLimits(-1.0, 1.0);
	
	m_rightPID.SetPID(inertP, inertI, inertD);
	m_rightPID.SetLimits(-1.0, 1.0);
	
	m_deadheadPID.SetPID(movingP, movingI, movingD);
	m_deadheadPID.SetLimits(-0.25, 0.25);
}

void DriveSystem::Drive()
{
	m_leftSpeed = limit(m_leftSpeed);
	m_rightSpeed = limit(m_rightSpeed);
	
	if (m_robot->GetLeftFrontMotor() != NULL)
		m_robot->GetLeftFrontMotor()->Set(m_leftSpeed);
	
	if (m_robot->GetLeftRearMotor() != NULL)
		m_robot->GetLeftRearMotor()->Set(m_leftSpeed);
	
	if (m_robot->GetRightFrontMotor() != NULL)
		m_robot->GetRightFrontMotor()->Set(-m_rightSpeed);
	
	if (m_robot->GetRightRearMotor() != NULL)
		m_robot->GetRightRearMotor()->Set(-m_rightSpeed);
	
#ifdef FEATURE_GEAR_SWITCH
	switch (m_gear)
	{
	case kLoGear:
		m_robot->GetGearSwitch()->Set(0);
		break;
	case kHiGear:
		m_robot->GetGearSwitch()->Set(1);
		break;
	}
#endif
}

void DriveSystem::Stop()
{
	m_leftSpeed = m_rightSpeed = 0.0;
}

bool DriveSystem::IsMoving()
{
	return (m_leftSpeed > 0.05 || m_leftSpeed < -0.05 ||
			m_rightSpeed > 0.05 || m_rightSpeed < -0.05);
}

bool DriveSystem::IsTurning()
{
	double delta = m_leftSpeed - m_rightSpeed;
	return (delta > 0.15 || delta < -0.15);
}

void DriveSystem::Compensate()
{
	if (!IsMoving())
	{
		if (m_firstInertComp)
		{
			m_firstInertComp = false;
			InitInertCompensate();
		}
		InertCompensate();
		m_firstMovingComp = true;
	}
	else
	{
		if (m_firstMovingComp)
		{
			m_firstMovingComp = false;
			InitMovingCompensate();
		}
		MovingCompensate();
		m_firstInertComp = true;
	}
	
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->UpdateLCD();
#endif
}

void DriveSystem::InitInertCompensate()
{
#ifdef FEATURE_DRIVE_ENCODERS
	m_robot->GetLeftDriveEncoder()->Reset();
	m_robot->GetRightDriveEncoder()->Reset();
#endif
	
	m_leftPID.Reset();
	m_leftPID.SetTarget(0.0);
	m_leftPID.Start();
	
	m_rightPID.Reset();
	m_rightPID.SetTarget(0.0);
	m_rightPID.Start();
}

void DriveSystem::InertCompensate()
{
	INT32 encoderL, encoderR;
	float encoderLAngle, encoderRAngle;
	int ticksPerRevolution = m_robot->GetConfig().SetDefault("driveEncoderTicksPerRev", 300);

#ifdef FEATURE_DRIVE_ENCODERS
	encoderL = m_robot->GetLeftDriveEncoder()->Get();
	encoderR = m_robot->GetRightDriveEncoder()->Get();
#else
	encoderL = encoderR = 0;
#endif
	
	encoderLAngle = (float)encoderL * 360.0 / ticksPerRevolution;
	encoderRAngle = (float)encoderR * 360.0 / ticksPerRevolution;
		
	m_leftPID.Update(encoderLAngle);
	m_rightPID.Update(encoderRAngle);
	
	if ((encoderLAngle > 5 && encoderLAngle < 180) || (encoderLAngle < -5 && encoderLAngle > -180))
		m_leftSpeed = m_leftPID.GetOutput();
	if ((encoderRAngle > 5 && encoderRAngle < 180) || (encoderRAngle < -5 && encoderRAngle > -180))
		m_rightSpeed = m_rightPID.GetOutput();
		
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->PrintfLine(DS_LCD::kUser_Line2, "PID Inert");
	lcd->PrintfLine(DS_LCD::kUser_Line4, "L: %.1f R: %.1f", encoderLAngle, encoderRAngle);
#endif
}

void DriveSystem::InitMovingCompensate()
{
	float angle;
	
#ifdef FEATURE_GYRO
	angle = m_robot->GetGyro()->GetAngle();
#else
	angle = 0;
#endif
	
	m_deadheadPID.Reset();
	m_deadheadPID.SetTarget(angle);
	m_deadheadPID.Start();
}

void DriveSystem::MovingCompensate()
{
	float angle;
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
	
#ifdef FEATURE_GYRO
	angle = m_robot->GetGyro()->GetAngle();
#else
	angle = 0;
#endif
	
	m_deadheadPID.Update(angle);
	
	m_leftSpeed = limit(m_leftSpeed + m_deadheadPID.GetOutput());
	m_rightSpeed = limit(m_rightSpeed - m_deadheadPID.GetOutput());
		
#ifdef FEATURE_LCD
	lcd->PrintfLine(DS_LCD::kUser_Line2, "PID: %.4f", m_deadheadPID.GetOutput());
#endif
}

// AUTONOMOUS

AutonomousDriveSystem::AutonomousDriveSystem(BossRobot *r)
    : DriveSystem(r)
{
}
	
void AutonomousDriveSystem::ReadControls()
{	
}

// ARCADE

ArcadeDriveSystem::ArcadeDriveSystem(BossRobot *r)
    : TeleoperatedDriveSystem(r)
{
	m_move = m_rotate = 0.0;
}

void ArcadeDriveSystem::ReadControls()
{
	m_move = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_rotate = -(ControlBoard::GetInstance().GetJoystick(2).GetX());
	
	if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(2))
		m_gear = kLoGear;
	else if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(3))
		m_gear = kHiGear;
	
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
	return (m_move > 0.05 || m_move < -0.05 || IsTurning());
}

bool ArcadeDriveSystem::IsTurning()
{
	return (m_rotate > 0.05 || m_rotate < -0.05);
}

// TANK

TankDriveSystem::TankDriveSystem(BossRobot *r)
    : TeleoperatedDriveSystem(r)
{
}

void TankDriveSystem::ReadControls()
{
	m_leftSpeed = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_rightSpeed = -(ControlBoard::GetInstance().GetJoystick(2).GetY());
	
	if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(2))
		m_gear = kLoGear;
	else if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(3))
		m_gear = kHiGear;
	
	InterpretControls();
}

void TankDriveSystem::InterpretControls()
{
	m_leftSpeed = limit(m_leftSpeed);
	m_rightSpeed = limit(m_rightSpeed);
	
	// Copied from WPILib
	if (1) // squaredInputs
	{
		m_leftSpeed = sign_square(m_leftSpeed);
		m_rightSpeed = sign_square(m_rightSpeed);
	}
}

// XBOX

/* 	Xbox controller info
 * 
 * 	Axes (1-based):
 * 		1 - Left X
 * 		2 - Left Y
 * 		3 - Both triggers
 * 		4 - Right X
 * 		5 - Right Y
 */
XboxDriveSystem::XboxDriveSystem(BossRobot *r)
    : ArcadeDriveSystem(r)
{
}

void XboxDriveSystem::ReadControls(void)
{
	m_move = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_rotate = -(ControlBoard::GetInstance().GetJoystick(1).GetRawAxis(4));
	
	if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(5))
		m_gear = kLoGear;
	else if (ControlBoard::GetInstance().GetJoystick(1).GetRawButton(6))
		m_gear = kHiGear;
	
	InterpretControls();
}

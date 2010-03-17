/**
 *	@file KickerSystem.cpp
 *	Implementation of the KickerSystem class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/16/10.
 */

#include "KickerSystem.hpp"
#include "ControlBoard.hpp"

KickerSystem::KickerSystem(BossRobot *r)
{
	m_robot = r;
	Reset();
}

KickerSystem::~KickerSystem()
{
}

void KickerSystem::Reset()
{
	m_strength = kStrengthLo;
	m_kicking = false;
	m_startedKicking = false;
	m_cocked = false;

	m_kickerPID.SetPID(m_robot->GetConfig().SetDefault("kickerP", 0.8),
					   m_robot->GetConfig().SetDefault("kickerI", 0.0),
					   m_robot->GetConfig().SetDefault("kickerD", 0.0));
	m_kickerPID.SetLimits(0.0, 1.0);
	m_kickerPID.SetTarget(m_robot->GetConfig().SetDefault("kickerRestAngle", 4.5));
}
	
void KickerSystem::ReadControls()
{
	ControlBoard &board = ControlBoard::GetInstance();
	
	if (board.GetButton(15))
		m_strength = kStrengthLo;
	else if (board.GetButton(11))
		m_strength = kStrengthMd;
	else if (board.GetButton(7))
		m_strength = kStrengthHi;
	
	m_kickTrigger.Set(board.GetJoystick(3).GetTrigger());
	if (m_kickTrigger.GetTriggeredOn())
		Kick();
	
	if (board.GetJoystick(3).GetRawButton(2) ||
		board.GetJoystick(3).GetRawButton(3) ||
		board.GetJoystick(3).GetRawButton(4) ||
		board.GetJoystick(3).GetRawButton(5))
	{
		m_intakeState = 1;
	}
	else if (board.GetJoystick(3).GetRawButton(8))
	{
		m_intakeState = -1;
	}
	else
	{
		m_intakeState = 0;
	}
	
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->PrintfLine(DS_LCD::kUser_Line5, "Kicker: %.2fV", m_robot->GetKickerEncoder()->GetVoltage());
	lcd->UpdateLCD();
#endif
}

void KickerSystem::Kick()
{
	m_kicking = true;
	m_robot->GetKickerEncoder()->ResetAccumulator();
}

void KickerSystem::Update()
{
	UpdateIntake();
	UpdateWinch();
	UpdateKicker();
	
	m_intakeFlag.ClearTrigger();
}

void KickerSystem::UpdateWinch()
{
	double target = 0.0;
	double actual, tolerance;
	int lightNum = 0;

	ControlBoard::GetInstance().SetMultiLight(6, ControlBoard::kLightRed);
	ControlBoard::GetInstance().SetMultiLight(14, ControlBoard::kLightRed);
	ControlBoard::GetInstance().SetMultiLight(10, ControlBoard::kLightRed);
	
	switch (m_strength)
	{
	case kStrengthLo:
		target = m_robot->GetConfig().SetDefault("kickerStrengthLo_pos", 0.0);
		lightNum = 6;
		break;
	case kStrengthMd:
		target = m_robot->GetConfig().SetDefault("kickerStrengthMd_pos", 0.0);
		lightNum = 14;
		break;
	case kStrengthHi:
		target = m_robot->GetConfig().SetDefault("kickerStrengthHi_pos", 0.0);
		lightNum = 10;
		break;
	default:
		// We entered a weird strength. Don't do anything, plz!
#ifdef FEATURE_UPPER_BOARD
		m_robot->GetKickerWinch1()->Set(Relay::kOff);
		m_robot->GetKickerWinch2()->Set(Relay::kOff);
#endif
		return;
	}

#ifdef FEATURE_UPPER_BOARD
	actual = m_robot->GetKickerWinchSensor()->GetVoltage();
	tolerance = m_robot->GetConfig().SetDefault("kickerPosTolerance", 0.02);
	if (actual < (target - tolerance))
	{
		m_robot->GetKickerWinch1()->Set(Relay::kForward);
		m_robot->GetKickerWinch2()->Set(Relay::kForward);
		if (lightNum != 0)
			ControlBoard::GetInstance().SetMultiLight(lightNum, ControlBoard::kLightYellow);
	}
	else if (actual > (target + tolerance))
	{
		m_robot->GetKickerWinch1()->Set(Relay::kReverse);
		m_robot->GetKickerWinch2()->Set(Relay::kReverse);
		if (lightNum != 0)
			ControlBoard::GetInstance().SetMultiLight(lightNum, ControlBoard::kLightYellow);
	}
	else
	{
		m_robot->GetKickerWinch1()->Set(Relay::kOff);
		m_robot->GetKickerWinch2()->Set(Relay::kOff);
		if (lightNum != 0)
			ControlBoard::GetInstance().SetMultiLight(lightNum, ControlBoard::kLightGreen);
	}
#else
	ControlBoard::GetInstance().SetMultiLight(lightNum, ControlBoard::kLightGreen);
#endif
}

void KickerSystem::UpdateKicker()
{
	AbsoluteEncoder *encoder = m_robot->GetKickerEncoder();
	float encoderVoltage, encoderMaxVoltage;
	double restVoltage, cockedVoltage, tol;
	
	encoderVoltage = encoder->GetIncrementalVoltage();
	encoderMaxVoltage = encoder->GetMaxVoltage();
	
	// Get config values
	restVoltage = m_robot->GetConfig().SetDefault("kickerRestAngle", 4.5);
	cockedVoltage = m_robot->GetConfig().SetDefault("kickerCockedAngle", 4.710);
	tol = m_robot->GetConfig().SetDefault("kickerPosTolerance", 0.05);

	m_kickerPID.SetPID(m_robot->GetConfig().GetDouble("kickerP"),
					   m_robot->GetConfig().GetDouble("kickerI"),
					   m_robot->GetConfig().GetDouble("kickerD"));
	
	// Determine target
	// Note that this relies on the fact that the cocked angle is farther than rest, so the motor won't
	// backpedal.
	if (m_kicking)
	{
		// This is our first kicking cycle.  Set the target.
		if (!m_startedKicking)
		{
			encoder->ResetAccumulator();
			encoderVoltage = encoder->GetIncrementalVoltage();
			if (encoderVoltage < restVoltage - tol)
			{
				m_kickerPID.SetTarget(restVoltage);
			}
			else
			{
				m_kickerPID.SetTarget(restVoltage + encoderMaxVoltage);
			}
		}
		m_startedKicking = true;
		
		// Check to see if we're finished kicking
		if (encoderVoltage > m_kickerPID.GetTarget() - tol)
		{
			// We've finished kicking. Clean up.
			m_kicking = false;
			m_startedKicking = false;
			m_cocked = false;
			encoder->ResetAccumulator();
			m_kickerPID.SetTarget(restVoltage);
		}
	}
	else if (m_intakeFlag.CheckTriggeredOn())
	{
		// Move the kicker to cocked if the operator starts the intake
		if (!m_cocked)
		{
			encoder->ResetAccumulator();
			encoderVoltage = encoder->GetIncrementalVoltage();
			m_kickerPID.SetTarget((cockedVoltage >= restVoltage)
								  ? cockedVoltage
								  : cockedVoltage + encoderMaxVoltage);
		}
		
		m_cocked = true;
	}
	else if (m_cocked)
	{
		if (encoderVoltage > m_kickerPID.GetTarget() - tol)
		{
			// Done cocking, hold position.
			encoder->ResetAccumulator();
			encoderVoltage = encoder->GetIncrementalVoltage();
			m_kickerPID.SetTarget(cockedVoltage);
		}
	}
	
	// Run the motor!
	m_robot->GetKickerMotor()->Set(m_kickerPID.Update(encoderVoltage));
}

void KickerSystem::UpdateIntake()
{
	switch (m_intakeState)
	{
	case -1:
		m_robot->GetIntakeMotor()->Set(-1.0);
		break;
	case 0:
		m_robot->GetIntakeMotor()->Set(0.0);
		break;
	case 1:
		m_robot->GetIntakeMotor()->Set(1.0);
		break;
	default:
		m_robot->GetIntakeMotor()->Set(0.0);
		break;
	}
	
	m_intakeFlag.Set(m_intakeState != 0);
}

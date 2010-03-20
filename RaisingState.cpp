/**
 *	@file RaisingState.cpp
 *	Implementation of the RaisingState class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/12/10.
 */

#include "State.hpp"
#include "DriveSystem.hpp"
#include "ArmSystem.hpp"
#include "SimplePID.hpp"
#include "ControlBoard.hpp"
#include <math.h>

RaisingState::RaisingState(BossRobot *r) : State(r)
{
	m_timer = NULL;
}

void RaisingState::Enter()
{
	ConfigParser &c = m_robot->GetConfig();
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->Clear();
	lcd->PrintfLine(DS_LCD::kUser_Line1, "Raising...");
	lcd->PrintfLine(DS_LCD::kUser_Line2, "User control disabled");
	lcd->UpdateLCD();
#endif
	
	m_robot->SoftStop();
	
	m_robot->GetArmSystem()->SetState(ArmSystem::kRaised);
	
	m_elbowPID.SetLimits(0.0, 1.0); // we have a ratchet on the drive now
	m_elbowPID.SetPID(c.SetDefault("elbowP", 7.5),
					  c.SetDefault("elbowI", 0.0),
					  c.SetDefault("elbowD", 0.0));
	m_elbowPID.Reset();
	m_elbowPID.SetTarget(c.SetDefault("elbowTarget", 1.0));
	m_elbowPID.Start();
	
	m_timer = new Timer();
	m_timer->Start();
}

void RaisingState::Exit()
{
	m_elbowPID.Stop();
	delete m_timer;
	m_timer = NULL;
}

void RaisingState::Step()
{
	float elbowVoltage = 5.0 - m_robot->GetElbowSensor()->GetVoltage();
	double output;
	
	if (fabs(elbowVoltage - m_elbowPID.GetTarget()) < m_robot->GetConfig().SetDefault("elbowTol", 0.01))
	{
		// We've raised ourselves.  Don't let the operators do anything.
		m_robot->ChangeState(new DisabledState(m_robot, NULL));
		return;
	}
	else if (ControlBoard::GetInstance().GetButton(2))
	{
		// Operator ordered a premature soft-disable.
		m_robot->ChangeState(new DisabledState(m_robot, this));
		return;
	}

	m_robot->GetGearSwitch()->Set(1);
	m_robot->GetArmSystem()->SetState(ArmSystem::kRaised);
	m_robot->GetArmSystem()->Brake();
	m_robot->GetArmSystem()->Update();
	
	if (m_timer->Get() < m_robot->GetConfig().SetDefault("quasiNeutralDelay", 0.1))
	{
		return;
	}

	m_robot->GetElbowSwitch()->Set(1);
	// Solenoid 3 high and solenoid 4 low (or vice versa)
	
	m_elbowPID.Update(elbowVoltage);
	DS_LCD::GetInstance()->PrintfLine(DS_LCD::kUser_Line3, "PID: %f", m_elbowPID.GetOutput());
	DS_LCD::GetInstance()->UpdateLCD();
	
	if (elbowVoltage > m_robot->GetConfig().SetDefault("elbowMinimum", 0.1) &&
		elbowVoltage < m_robot->GetConfig().SetDefault("elbowMaximum", 4.9))
	{
		output = m_elbowPID.GetOutput();
		m_robot->GetLeftFrontDriveMotor()->Set(output);
		m_robot->GetLeftRearDriveMotor()->Set(output);
		m_robot->GetRightFrontDriveMotor()->Set(-output);
		m_robot->GetRightRearDriveMotor()->Set(-output);
	}
	else
	{
		m_robot->GetLeftFrontDriveMotor()->Set(0.0);
		m_robot->GetLeftRearDriveMotor()->Set(0.0);
		m_robot->GetRightFrontDriveMotor()->Set(0.0);
		m_robot->GetRightRearDriveMotor()->Set(0.0);
	}
}

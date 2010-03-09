/**
 *	@file ArmSystem.cpp
 *	Implementation of the ArmSystem class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/8/10.
 */

#include "ArmSystem.hpp"

ArmSystem::ArmSystem(BossRobot *r)
{
	m_robot = r;
	m_state = kStowed;
	m_pidControl.SetPID(m_robot->GetConfig().SetDefault("shoulderP", 5.0),
						m_robot->GetConfig().SetDefault("shoulderI", 0.0),
						m_robot->GetConfig().SetDefault("shoulderD", 0.0));
	m_pidControl.SetLimits(-1.0, 1.0);
	m_pidControl.Start();
}

ArmSystem::~ArmSystem()
{
	m_pidControl.Stop();
}

void ArmSystem::Update()
{
	ConfigParser &config = m_robot->GetConfig();
	float shoulderVoltage = m_robot->GetShoulderSensor()->GetVoltage();
	
	switch (m_state)
	{
	case kStowed:
		m_pidControl.SetTarget(config.SetDefault("shoulderStowedPos", 0.1));
		break;
	case kRaised:
		m_pidControl.SetTarget(config.SetDefault("shoulderRaisedPos", 1.6));
		break;
	case kGTFU:
		m_pidControl.SetTarget(config.SetDefault("shoulderGTFUPos", 2.0));
		break;
	default:
		m_pidControl.SetTarget(shoulderVoltage);
		break;
	}
	
	m_pidControl.Update(shoulderVoltage);
	
	if (shoulderVoltage > config.SetDefault("shoulderMinPos", 0.1) &&
		shoulderVoltage < config.SetDefault("shoulderMaxPos", 4.9))
	{
		m_robot->GetShoulderMotor1()->Set(-m_pidControl.GetOutput());
		m_robot->GetShoulderMotor2()->Set(-m_pidControl.GetOutput());
	}
	else
	{
		// We are out of acceptable bounds, stop the motors to prevent damage.
		m_robot->GetShoulderMotor1()->Set(0.0);
		m_robot->GetShoulderMotor2()->Set(0.0);
	}
}

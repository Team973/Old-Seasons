//
//	ConfigState.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/26/10.
//

#include "ConfigState.hpp"
#include "NormalState.hpp"
#include "ControlBoard.hpp"

ConfigState::ConfigState(BossRobot *r)
	: State(r)
{
}

void ConfigState::Enter()
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
	lcd->PrintfLine(DS_LCD::kUser_Line1, "CONFIGURATION");
	lcd->UpdateLCD();
#endif
	
	m_prevReread = false;
	m_prevStrengthLo = m_prevStrengthMd = m_prevStrengthHi = false;
	m_robot->GetConfig().Read("boss.cfg");
}

void ConfigState::Exit()
{
	m_robot->GetConfig().Write("boss.cfg", "This is the main configuration file and was automatically generated.");
}

void ConfigState::Step()
{
	// Check for state stopping
	if (!ControlBoard::GetInstance().GetButton(1))
	{
		m_robot->ChangeState(new NormalState(m_robot));
		return;
	}
	
	// Check for the re-read config file button
	if (ControlBoard::GetInstance().GetButton(5) && !m_prevReread)
	{
		m_robot->GetConfig().Read("boss.cfg");
	}
	m_prevReread = ControlBoard::GetInstance().GetButton(5);
	
	HandleStrengthPresetting();
}

#ifdef FEATURE_UPPER_BOARD
#define STRENGTH_PRESET(button, name) \
	{ \
		if (ControlBoard::GetInstance().GetButton(button) && !m_prevStrength##name) \
			m_robot->GetConfig().Set("kickerStrength" #name "_pos", m_robot->GetKickerWinchSensor()->GetVoltage()); \
		m_prevStrength##name = ControlBoard::GetInstance().GetButton(button); \
	}
#else
#define STRENGTH_PRESET(button, name)
#endif

void ConfigState::HandleStrengthPresetting()
{
	STRENGTH_PRESET(15, Lo);
	STRENGTH_PRESET(11, Md);
	STRENGTH_PRESET(7,  Hi);
	
#ifdef FEATURE_UPPER_BOARD
	if (ControlBoard::GetInstance().GetJoystick(3).GetRawButton(6))
	{
		m_robot->GetKickerWinch1()->Set(Relay::kForward);
		m_robot->GetKickerWinch2()->Set(Relay::kForward);
	}
	else if (ControlBoard::GetInstance().GetJoystick(3).GetRawButton(7))
	{
		m_robot->GetKickerWinch1()->Set(Relay::kReverse);
		m_robot->GetKickerWinch2()->Set(Relay::kReverse);
	}
	else
	{
		m_robot->GetKickerWinch1()->Set(Relay::kOff);
		m_robot->GetKickerWinch2()->Set(Relay::kOff);
	}
#endif
}

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
	
	m_reread = Flag();
	m_strengthLo = m_strengthMd = m_strengthHi = Flag();
	m_kickRest = Flag();
	m_robot->GetConfig().Read("boss.cfg");
}

void ConfigState::Exit()
{
	m_robot->GetConfig().Write("boss.cfg", "This is the main configuration file and was automatically generated.");
}

void ConfigState::Step()
{
	ControlBoard &board = ControlBoard::GetInstance();
	
	// Check for state stopping
	if (!board.GetButton(1))
	{
		m_robot->ChangeState(new NormalState(m_robot));
		return;
	}
	
	// Check for the re-read config file button
	m_reread.Set(board.GetButton(5));
	if (m_reread.GetTriggeredOn())
	{
		m_robot->GetConfig().Read("boss.cfg");
	}
	
	HandleStrengthPresetting();
	
	// Check for kick config
	m_kickRest.Set(board.GetJoystick(3).GetTrigger());
	if (m_kickRest.GetTriggeredOn())
	{
		m_robot->GetConfig().Set("kickerRestAngle", m_robot->GetKickerEncoder()->GetVoltage());
	}
}

#ifdef FEATURE_UPPER_BOARD
#define STRENGTH_PRESET(button, name) \
	{ \
		m_strength##name.Set(ControlBoard::GetInstance().GetButton(button)); \
		if (m_strength##name.GetTriggeredOn()) \
			m_robot->GetConfig().Set("kickerStrength" #name "_pos", m_robot->GetKickerWinchSensor()->GetVoltage()); \
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

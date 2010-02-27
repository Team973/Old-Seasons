//
//	KickerSystem.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
//

#include "KickerSystem.hpp"
#include "ControlBoard.hpp"

KickerSystem::KickerSystem(BossRobot *r)
{
	m_robot = r;
	m_strength = kStrengthLo;
}

KickerSystem::~KickerSystem()
{
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
}

void KickerSystem::Update()
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

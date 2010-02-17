//
//	KickerSystem.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
//

#include "KickerSystem.hpp"
#include "ControlBoard.hpp"

KickerSystem::KickerSystem()
{
	m_strength = 1;
}

KickerSystem::~KickerSystem()
{
}
	
void KickerSystem::ReadControls()
{
	ControlBoard &board = ControlBoard::GetInstance();
	
	if (board.GetButton(1))
		m_strength = kStrengthLo;
	else if (board.GetButton(2))
		m_strength = kStrengthMd;
	else if (board.GetButton(3))
		m_strength = kStrengthHi;
}

void KickerSystem::Update()
{
	double target = 0.0;
	double actual, tolerance;
	
	switch (m_strength)
	{
	case kStrengthLo:
		target = m_robot->GetConfig().SetDefault("kickerStrengthLo_pos", 0.0);
		break;
	case kStrengthMd:
		target = m_robot->GetConfig().SetDefault("kickerStrengthMd_pos", 0.0);
		break;
	case kStrengthHi:
		target = m_robot->GetConfig().SetDefault("kickerStrengthHi_pos", 0.0);
		break;
	default:
		// We entered a weird strength. Don't do anything, plz!
		m_robot->GetKickerWinch1()->Set(Relay::kOff);
		m_robot->GetKickerWinch2()->Set(Relay::kOff);
		return;
	}
	
	actual = m_robot->GetKickerWinchSensor()->GetVoltage();
	tolerance = m_robot->GetConfig().SetDefault("kickerPosTolerance", 0.5);
	if (actual < (target - tolerance))
		m_robot->GetKickerWinch1()->Set(Relay::kForward);
	else if (actual > (target + tolerance))
		m_robot->GetKickerWinch2()->Set(Relay::kReverse);
}

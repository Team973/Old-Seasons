//
//	KickerSystem.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
//

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"

#ifndef _BOSS_973_KICKERSYSTEM_H_
#define _BOSS_973_KICKERSYSTEM_H_

class KickerSystem
{
protected:
	BossRobot *m_robot;
	short m_strength;
	
	enum {kStrengthLo, kStrengthMd, kStrengthHi};
public:
	KickerSystem();
	virtual ~KickerSystem();
	
	virtual void ReadControls();
	virtual void Update();
};

#endif

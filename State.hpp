//
//	State.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"
#include "BossRobot.hpp"

#ifndef _BOSS_973_STATE_H_
#define _BOSS_973_STATE_H_

class State
{
protected:
	BossRobot *m_robot;
	
	State()                { m_robot = NULL; }
	State(BossRobot *r)    { m_robot = r; }
public:
	virtual ~State() {};
	
	/**
	 *	Enter the state.
	 *
	 *	This will be called once when the State is initially changed into (via
	 *	BossRobot::ChangeState), but the same instance may be entered several
	 *	times during the execution of the program.  This is guaranteed to be
	 *	called before State::Step.
	 */
	virtual void Enter() = 0;
	
	/**
	 *	Exit the state.
	 *
	 *	This will be called once when the BossRobot class is changing to a new
	 *	state (via BossRobot::ChangeState), but the same instance may be exited
	 *	several times during the execution of the program.  This is guaranteed
	 *	to be called before the new state's State::Enter.
	 */
	virtual void Exit() = 0;
	
	/**
	 *	Do the state's logic.
	 *
	 *	BossRobot calls this method once for every iteration of the run loop
	 *	when the state is active.
	 */
	virtual void Step() = 0;
};

// TODO: Expand acronym without offending people
class GTFUState : public State
{
};

class FinaleState : public State
{
};

class RaisingState : public State
{
};

#endif

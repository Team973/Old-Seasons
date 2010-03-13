/**
 *	@file State.hpp
 *	The interface for the robot state machine
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "WPILib.h"
#include "BossRobot.hpp"
#include "SimplePID.hpp"
#include "Flag.hpp"

#ifndef _BOSS_973_STATE_H_
#define _BOSS_973_STATE_H_

/**
 *	A certain behavior of the robot.
 *
 *	In laymen's terms, each state is a different mode of the robot.  This class
 *	merely defines the interface that the states must implement so that the
 *	BossRobot knows how to give control to the state.
 */
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

/**
 * The "Get The Frame Upright" state! (TM)
 */
class GTFUState : public State
{
};

/**
 *	"User wants to suspend" state.
 *
 *	This makes the arm go up and disables kicking.
 */
class FinaleState : public State
{
public:
	FinaleState(BossRobot *);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
};

class RaisingState : public State
{
private:
	SimplePID m_elbowPID;
public:
	RaisingState(BossRobot *);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
};

class DisabledState : public State
{
private:
	State *m_prevState;
	Flag m_switch;
public:
	DisabledState(BossRobot *, State *);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
};

#endif

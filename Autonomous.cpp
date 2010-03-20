/**
 *	@file Autonomous.cpp
 *	Implementation of the autonomous mode.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/19/10.
 */

#include "Autonomous.hpp"
#include "DriveSystem.hpp"

void MainAutonomous(BossRobot *robot)
{
	double autoDist;
	
	SetupAutonomous(robot);
	
	autoDist = robot->GetConfig().SetDefault("autonomousMaxDistance");
	
	while (robot->GetLeftDriveEncoder()->GetDistance() < autoDist &&
		   robot->GetRightDriveEncoder()->GetDistance() < autoDist)
	{
		// Run drive
		// Drive forward
		robot->GetDriveSystem()->Turn(0.2, 0.0);
		robot->GetDriveSystem()->Update();
		
		// Run kicker system
		// Kick if we have a ball
		robot->GetKickerSystem()->RunIntake();
		if (robot->GetKickerSystem()->HasPossession())
		{
			robot->GetKickerSystem()->Kick();
		}
		robot->GetKickerSystem()->Update();
	}
	
	// Stop
	robot->GetDriveSystem()->Stop();
	robot->GetDriveSystem()->Update();
}

void CalibrateEncoderAutonomous(BossRobot *robot)
{
	UINT32 tickDist = 300 * 10; // 10 revolutions
	
	SetupAutonomous(robot);
	
	while (robot->GetLeftDriveEncoder()->GetRaw() < tickDist * 4 &&
		   robot->GetRightDriveEncoder()->GetRaw() < tickDist * 4)
	{
		// Run drive forward
		robot->GetDriveSystem()->Turn(0.5, 0.0);
		robot->GetDriveSystem()->Update();
	}
	
	// Stop
	robot->GetDriveSystem()->Stop();
	robot->GetDriveSystem()->Update();
}

void SetupAutonomous(BossRobot *robot)
{
	robot->SetDriveSystem(new AutonomousDriveSystem(robot));
}

void DriveFor(BossRobot *robot, double time, double speed, double curve)
{
}

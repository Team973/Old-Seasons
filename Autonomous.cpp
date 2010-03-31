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
#include "KickerSystem.hpp"
#include "Options.hpp"

void MainAutonomous(BossRobot *robot)
{
	double autoDist, jogBackTime = -1.0;
	Timer t;
	bool started = false;
	
	SetupAutonomous(robot);
	
	// Wind up winch
//	robot->GetKickerSystem()->SetStrength(KickerSystem::kStrengthHi);
//	robot->GetKickerSystem()->Cock();
//	while (robot->GetKickerSystem()->NeedsWinchUpdate())
//	{
//		robot->GetKickerSystem()->Update();
//	}
	
	autoDist = robot->GetConfig().SetDefault("autonomousMaxDistance", 12 * 12);
	robot->GetKickerSystem()->Reset();
	robot->GetKickerSystem()->Cock();
	robot->GetKickerSystem()->RunIntake();
	
	// Main loop
	t.Start();
	robot->GetDriveSystem()->Turn(0.2, 0.0);
	while (robot->GetLeftDriveEncoder()->GetDistance() < autoDist &&
		   robot->GetRightDriveEncoder()->GetDistance() < autoDist &&
		   robot->IsAutonomous())
	{
		robot->GetShoulderBrake()->Set(1); // to unbrake
#ifdef FEATURE_COMPRESSOR
		robot->GetCompressor()->Set(robot->GetPressureSwitch()->Get() ? Relay::kOff : Relay::kOn);
#endif
		
		if (!started)
		{
			if (t.Get() > 3.0)
			{
				started = true;
				t.Reset();
			}
			else
			{
				continue;
			}
		}
		
		// Run drive
		robot->GetDriveSystem()->Drive();
		
		// Run kicker system
		// Kick if we have a ball
		if (jogBackTime >= 0)
		{
			if (t.Get() - jogBackTime >= 1.0)
			{
				robot->GetKickerSystem()->Kick();
				robot->GetDriveSystem()->Stop();
				t.Reset();
				jogBackTime = -1.0;
			}
		}
		else if (t.Get() > 1.0 && robot->GetKickerSystem()->HasPossession())
		{
			jogBackTime = t.Get();
			robot->GetDriveSystem()->Turn(-0.2, 0.0);
		}
		else if (!robot->GetKickerSystem()->IsKicking())
		{
			robot->GetKickerSystem()->Cock();
			robot->GetDriveSystem()->Turn(0.2, 0.0);
			jogBackTime = -1.0;
		}
		robot->GetKickerSystem()->Update();
	}
	
	// Stop
	robot->GetDriveSystem()->Stop();
	robot->GetDriveSystem()->Drive();
}

void CalibrateEncoderAutonomous(BossRobot *robot)
{
	INT32 tickDist = 300 * 10; // 10 revolutions
	
	SetupAutonomous(robot);
	
	while (robot->GetLeftDriveEncoder()->GetRaw() < tickDist * 4 &&
		   robot->GetRightDriveEncoder()->GetRaw() < tickDist * 4)
	{
		// Run drive forward
		robot->GetDriveSystem()->Turn(0.5, 0.0);
		robot->GetDriveSystem()->Drive();
	}
	
	// Stop
	robot->GetDriveSystem()->Stop();
	robot->GetDriveSystem()->Drive();
}

void SetupAutonomous(BossRobot *robot)
{
	robot->SetDriveSystem(new AutonomousDriveSystem(robot));
	robot->GetLeftDriveEncoder()->Reset();
	robot->GetRightDriveEncoder()->Reset();
}

void DriveFor(BossRobot *robot, double time, double speed, double curve)
{
}

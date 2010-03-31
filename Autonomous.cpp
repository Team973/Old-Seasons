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
	short strength;
	Timer t;
	bool started = false;
	
	switch (robot->GetAutoValueB())
	{
	case 1:
		strength = KickerSystem::kStrengthLo;
		break;
	case 2:
		strength = KickerSystem::kStrengthMd;
		break;
	case 3:
		strength = KickerSystem::kStrengthHi;
		break;
	default:
		strength = KickerSystem::kStrengthMd;
		break;
	}
	
	// Set up everything!
	SetupAutonomous(robot);

	robot->GetKickerSystem()->Reset();
	robot->GetKickerSystem()->SetStrength(strength);
	robot->GetKickerSystem()->Cock();
	
	autoDist = robot->GetConfig().SetDefault("autonomousMaxDistance", 12 * 12);
	
	// Main loop
	t.Start();
	robot->GetDriveSystem()->Turn(0.2, 0.0);
	while (robot->GetLeftDriveEncoder()->GetDistance() < autoDist &&
		   robot->GetRightDriveEncoder()->GetDistance() < autoDist &&
		   robot->IsAutonomous() && !robot->IsDisabled())
	{
		robot->GetShoulderBrake()->Set(1); // to unbrake
#ifdef FEATURE_COMPRESSOR
		robot->GetCompressor()->Set(robot->GetPressureSwitch()->Get() ? Relay::kOff : Relay::kOn);
#endif
		robot->GetKickerSystem()->Update();
		
		if (!started)
		{
			if (t.Get() > (float)robot->GetAutoValueA())
			{
				started = true;
				robot->GetKickerSystem()->RunIntake();
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
	robot->GetDriveSystem()->Stop();
	robot->GetDriveSystem()->Drive();
	robot->GetKickerSystem()->Reset();
}

void DriveFor(BossRobot *robot, double time, double speed, double curve)
{
}

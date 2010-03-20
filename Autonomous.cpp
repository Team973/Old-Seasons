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
	double autoDist;
	Timer t;
	
	SetupAutonomous(robot);
	
	// Wind up winch
//	robot->GetKickerSystem()->SetStrength(KickerSystem::kStrengthHi);
//	robot->GetKickerSystem()->Cock();
//	while (robot->GetKickerSystem()->NeedsWinchUpdate())
//	{
//		robot->GetKickerSystem()->Update();
//	}
	
	// Run compressor
#ifdef FEATURE_COMPRESSOR
	robot->GetCompressor()->Set(Relay::kOn);
	while (!robot->GetPressureSwitch()->Get())
		;
	robot->GetCompressor()->Set(Relay::kOff);
#endif
	
	autoDist = robot->GetConfig().SetDefault("autonomousMaxDistance", 12 * 12);
	robot->GetKickerSystem()->Reset();
	robot->GetKickerSystem()->RunIntake();
	
	// Main loop
	t.Start();
	while (robot->GetLeftDriveEncoder()->GetDistance() < autoDist &&
		   robot->GetRightDriveEncoder()->GetDistance() < autoDist)
	{
		robot->GetShoulderBrake()->Set(1); // to unbrake
#ifdef FEATURE_COMPRESSOR
		robot->GetCompressor()->Set(robot->GetPressureSwitch()->Get() ? Relay::kOff : Relay::kOn);
#endif
		
		// Run drive
		// Drive forward
		robot->GetDriveSystem()->Turn(0.2, 0.0);
		robot->GetDriveSystem()->Drive();
		
		// Run kicker system
		// Kick if we have a ball
		if (t.Get() > 2.0 && robot->GetKickerSystem()->HasPossession())
		{
			robot->GetKickerSystem()->Kick();
			t.Reset();
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
}

void DriveFor(BossRobot *robot, double time, double speed, double curve)
{
}

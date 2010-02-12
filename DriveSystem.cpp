// DriveSystem.cpp
// Created on 2/11/10.

#include "DriveSystem.hpp"
#include "ControlBoard.hpp"

DriveSystem::DriveSystem()
{
	m_drive = NULL;
}

DriveSystem::DriveSystem(RobotDrive *d)
{
	m_drive = d;
}

void DriveSystem::Stop()
{
	m_drive->Drive(0.0, 0.0);
}

void DriveSystem::Compensate()
{
	// TODO
}

AutonomousDriveSystem::AutonomousDriveSystem(RobotDrive *d) : DriveSystem(d)
{
}
	
void AutonomousDriveSystem::ReadControls()
{	
}

void AutonomousDriveSystem::Drive()
{
}

ArcadeDriveSystem::ArcadeDriveSystem(RobotDrive *d) : DriveSystem(d)
{
	m_x = m_y = 0.0;
}

void ArcadeDriveSystem::ReadControls()
{
	m_y = -(m_robot->GetControlBoard()->GetJoystick(1).GetY());
	m_x = -(m_robot->GetControlBoard()->GetJoystick(2).GetX());
}

void ArcadeDriveSystem::Drive()
{
	m_drive->ArcadeDrive(m_y, m_x);
}

/* 	Xbox controller info
 * 
 * 	Axes (1-based):
 * 		1 - Left X
 * 		2 - Left Y
 * 		3 - Both triggers
 * 		4 - Right X
 * 		5 - Right Y
 */
XboxDriveSystem::XboxDriveSystem(RobotDrive *d) : ArcadeDriveSystem(d)
{
}

void XboxDriveSystem::ReadControls(void)
{
	m_y = -(m_robot->GetControlBoard()->GetJoystick(1).GetY());
	m_x = -(m_robot->GetControlBoard()->GetJoystick(1).GetRawAxis(4));
}

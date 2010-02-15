//
//	DriveSystem.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "DriveSystem.hpp"
#include "ControlBoard.hpp"

DriveSystem::DriveSystem()
{
	m_drive = NULL;
}

DriveSystem::DriveSystem(BossRobot *r, RobotDrive *d)
{
    m_robot = r;
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

AutonomousDriveSystem::AutonomousDriveSystem(BossRobot *r, RobotDrive *d)
    : DriveSystem(r, d)
{
}
	
void AutonomousDriveSystem::ReadControls()
{	
}

void AutonomousDriveSystem::Drive()
{
}

ArcadeDriveSystem::ArcadeDriveSystem(BossRobot *r, RobotDrive *d)
    : TeleoperatedDriveSystem(r, d)
{
	m_x = m_y = 0.0;
}

void ArcadeDriveSystem::ReadControls()
{
	m_y = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_x = -(ControlBoard::GetInstance().GetJoystick(2).GetX());
}

void ArcadeDriveSystem::Drive()
{
	m_drive->ArcadeDrive(m_y, m_x);
}

bool ArcadeDriveSystem::IsMoving()
{
	return (m_y > 0.25 || m_y < -0.25 ||
			m_x > 0.25 || m_x < -0.25);
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
XboxDriveSystem::XboxDriveSystem(BossRobot *r, RobotDrive *d)
    : ArcadeDriveSystem(r, d)
{
}

void XboxDriveSystem::ReadControls(void)
{
	m_y = -(ControlBoard::GetInstance().GetJoystick(1).GetY());
	m_x = -(ControlBoard::GetInstance().GetJoystick(1).GetRawAxis(4));
}

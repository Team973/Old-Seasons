// BossRobot.cpp
// Created on 2/11/10.

#include "BossRobot.hpp"

#include "State.hpp"
#include "ControlBoard.hpp"
#include "DriveSystem.hpp"

const float TELEOP_LOOP_LAG = 0.005;

BossRobot::BossRobot(void)
{
#ifdef FEATURE_LCD
	DriverStationLCD *lcd = DriverStationLCD::GetInstance();
#endif
	
	GetWatchdog().SetExpiration(0.25);

#ifdef FEATURE_LCD
	lcd->Printf(DriverStationLCD::kUser_Line1, 1, "Robot init            ");
	lcd->UpdateLCD();
#endif
	
	/* Drive system */
	m_leftMotor1 = new Jaguar(1);
	m_leftMotor2 = new Jaguar(3);
	m_rightMotor1 = new Jaguar(4);
	m_rightMotor2 = new Jaguar(2);
	
	m_driveSystem = new AutonomousDriveSystem(new RobotDrive(
			m_leftMotor1, m_leftMotor2, m_rightMotor1, m_rightMotor2));
	
	/* Pneumatics */
	m_compressor = new Relay(1, Relay::kForwardOnly);
	m_pressureSwitch = new DigitalInput(1);
	m_gearSwitch = new Solenoid(1);
	
	/* Misc */
	m_ioTimer = new Timer();
	m_ioTimer->Start();
	m_visionTimer = new Timer();
	m_visionTimer->Start();
	
	/* Camera */
#ifdef FEATURE_CAMERA
	// Camera setup
	GetWatchdog().SetEnabled(false);
	Wait(5.0);
#ifdef FEATURE_LCD
	lcd->Printf(DriverStationLCD::kUser_Line1, 1, "Done waiting for cam  ");
	lcd->UpdateLCD();
#endif

	m_camera = &(AxisCamera::GetInstance());
	m_camera->WriteResolution(AxisCameraParams::kResolution_320x240);
	m_camera->WriteBrightness(0);
	m_camera->WriteCompression(75);
	m_camera->WriteMaxFPS(15);
	
	// Tell the operator we're just idling
#ifdef FEATURE_LCD
	lcd->Printf(DriverStationLCD::kUser_Line1, 1, "Camera initialized    ");
	lcd->UpdateLCD();
	Wait(1.0);
#endif
	GetWatchdog().SetEnabled(true);
#endif

#ifdef FEATURE_LCD
	lcd->Printf(DriverStationLCD::kUser_Line1, 1, "Robot ready           ");
	lcd->UpdateLCD();
#endif
}

/**
 * Drive left & right motors for 2 seconds then stop
 */
void BossRobot::Autonomous(void)
{
	GetWatchdog().SetEnabled(false);
//	m_drive->Drive(0.5, 0.0); 	// drive forwards half speed
//	Wait(2.0); 				//    for 2 seconds
//	m_drive->Drive(0.0, 0.0); 	// stop robot
}

/**
 * Runs the motors with arcade steering. 
 */
void BossRobot::OperatorControl(void)
{
	// Main loop
	GetWatchdog().SetEnabled(true);
	GetWatchdog().Feed();
	
	while (IsOperatorControl())
	{
		GetWatchdog().SetEnabled(true);
		GetWatchdog().Feed();
		
		m_state->Step();
		
		// Post-iteration clean up
		GetWatchdog().Feed();
		Wait(TELEOP_LOOP_LAG);				// wait for a motor update time
		GetWatchdog().Feed();
	}
}

RobotDrive *BossRobot::GetDrive()
{
	return m_driveSystem->GetDrive();
}

void BossRobot::SendVisionData()
{
	if (m_visionTimer->Get() < 0.1)
		return;
	m_visionTimer->Reset();
	Dashboard &dash = DriverStation::GetInstance()->GetHighPriorityDashboardPacker();
	dash.AddCluster(); // wire (2 elements)
	{
		dash.AddCluster(); // tracking data
		{
			dash.AddDouble(0.0); // Joystick X
			dash.AddDouble(0.0); // angle
			dash.AddDouble(0.0); // angular rate
			dash.AddDouble(0.0); // other X
		}
		dash.FinalizeCluster();
		dash.AddCluster(); // target Info (2 elements)
		{
			dash.AddArray(); // targets
			{
//                for (unsigned i = 0; i < targets.size(); i++) {
//                    dash.AddCluster(); // targets
//                    {
//                        dash.AddDouble(targets[i].m_score); // target score
//                        dash.AddCluster(); // Circle Description (5 elements)
//                        {
//                            dash.AddCluster(); // Position (2 elements)
//                            {
//                                dash.AddFloat((float) (targets[i].m_xPos / targets[i].m_xMax)); // X
//                                dash.AddFloat((float) targets[i].m_yPos); // Y
//                            }
//                            dash.FinalizeCluster();
//
//                            dash.AddDouble(targets[i].m_rotation); // Angle
//                            dash.AddDouble(targets[i].m_majorRadius); // Major Radius
//                            dash.AddDouble(targets[i].m_minorRadius); // Minor Radius
//                            dash.AddDouble(targets[i].m_rawScore); // Raw score
//                            }
//                        dash.FinalizeCluster(); // Position
//                        }
//                    dash.FinalizeCluster(); // targets
//                    }
			}
			dash.FinalizeArray();
			
			dash.AddU32((int) 0); // Timestamp

		}
		dash.FinalizeCluster(); // target Info
	}
	dash.FinalizeCluster(); // wire
	dash.Finalize();
}

void BossRobot::SendIOPortData()
{
	if (m_ioTimer->Get() < 0.1)
		return;
	m_ioTimer->Reset();
	Dashboard &dash = DriverStation::GetInstance()->GetLowPriorityDashboardPacker();
	dash.AddCluster();
	{
		dash.AddCluster();
		{ //analog modules 
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
					dash.AddFloat((float)AnalogModule::GetInstance(1)->GetAverageVoltage(i));
				}
			}
			dash.FinalizeCluster();
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
//					dash.AddFloat((float)AnalogModule::GetInstance(2)->GetAverageVoltage(i));
					dash.AddFloat(0.0);
				}
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		dash.AddCluster();
		{ //digital modules
			dash.AddCluster();
			{
				dash.AddCluster();
				{
					int module = 4;
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayReverse());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
							dash.AddU8((unsigned char)DigitalModule::GetInstance(module)->GetPWM(i));
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();

			dash.AddCluster();
			{
				dash.AddCluster();
				{
//					int module = 6;
					dash.AddU8(0);
					dash.AddU8(0);
					dash.AddU16(0);
					dash.AddU16(0);
//					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
//					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
//					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
//					dash.AddU16(DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
//							dash.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
							dash.AddU8(0);
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		// Solenoids (must have objects for each)
		//dash.AddU8((unsigned char)m_gearSwitch->Get());
		dash.AddU8(m_still ? 0x01 : 0x00);
	}
	dash.FinalizeCluster();
	dash.Finalize();
}

START_ROBOT_CLASS(BossRobot);

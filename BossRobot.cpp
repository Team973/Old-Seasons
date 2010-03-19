/**
 *	@file BossRobot.cpp
 *	Implementation of the BossRobot class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "BossRobot.hpp"

#include "State.hpp"
#include "NormalState.hpp"
#include "DriveSystem.hpp"
#include "KickerSystem.hpp"
#include "ArmSystem.hpp"

#include <iostream>
#include <fstream>

const float TELEOP_LOOP_LAG = 0.005;

extern "C" int Priv_SetWriteFileAllowed(UINT32 enable);

BossRobot::BossRobot(void)
{
#ifdef FEATURE_LCD
	DS_LCD *lcd = DS_LCD::GetInstance();
#endif
	
	GetWatchdog().SetExpiration(0.25);

#ifdef FEATURE_LCD
	lcd->PrintfLine(DS_LCD::kUser_Line1, "Robot init");
	lcd->UpdateLCD();
#endif
	
	/* Program setup */
	m_state = m_prevState = NULL;
	
	/* Config file */
	Priv_SetWriteFileAllowed(1);
	
	m_config.Read("boss.cfg");
	
	/* Drive system */
#ifdef FEATURE_DRIVE_VICTORS
	// Adam indicates that the 3rd PWM pin is dead.
	// Luckily, we're flexible.
	m_leftMotor1 = new Victor(1);
	m_leftMotor2 = new Victor(5);
	m_rightMotor1 = new Victor(4);
	m_rightMotor2 = new Victor(2);
#else
	m_leftMotor1 = new Jaguar(1);
	m_leftMotor2 = new Jaguar(3);
	m_rightMotor1 = new Jaguar(4);
	m_rightMotor2 = new Jaguar(2);
#endif
	
	m_driveSystem = new AutonomousDriveSystem(this);
	
#ifdef FEATURE_DRIVE_ENCODERS
	m_leftDriveEncoder = new Encoder(2, 3, true);
	m_leftDriveEncoder->Start();
	m_rightDriveEncoder = new Encoder(4, 5);
	m_rightDriveEncoder->Start();
#else
	m_leftDriveEncoder = m_rightDriveEncoder = NULL;
#endif
	
#ifdef FEATURE_GYRO
	m_gyroChannel = new AnalogChannel(1, 1);
	m_gyro = new Gyro(m_gyroChannel);
	m_gyro->SetSensitivity(0.006);
	m_gyro->Reset();
#else
	m_gyroChannel = NULL;
	m_gyro = NULL;
#endif
	
	/* Pneumatics */
	m_compressor = new Relay(1, Relay::kForwardOnly);
	m_pressureSwitch = new DigitalInput(1);
	m_gearSwitch = new Solenoid(1);
	
	/* Upper board */
#ifdef FEATURE_UPPER_BOARD
	m_shoulderMotor1 = new Victor(6, 1);
	m_shoulderMotor2 = new Victor(6, 2);
	m_shoulderSensor = new AnalogChannel(1, 4);
	m_shoulderBrakeSolenoid = new Solenoid(5);
	
	m_elbowSwitch = new Solenoid(2);
	m_elbowSensor = new AnalogChannel(1, 5);
	
	m_intakeMotor = new Victor(6, 3);
	m_intakeEncoder = new Encoder(6, 1, 6, 2);
	m_intakeEncoder->SetDistancePerPulse(1.0);
	
	m_kickerWinchSensor = new AnalogChannel(1, 3);
	m_kickerWinchRelay1 = new Relay(6, 3);
	m_kickerWinchRelay2 = new Relay(6, 4);
	m_kickerMotor = new Victor(6, 5);
	m_kickerEncoder = new AbsoluteEncoder(1, 2, m_config.SetDefault("kickerEncoderVoltage", 5.0));
#endif
	
	m_kickerSystem = new KickerSystem(this);
	m_armSystem = new ArmSystem(this);
	
	/* Misc */
	m_ioTimer = new Timer();
	m_ioTimer->Start();
	m_visionTimer = new Timer();
	m_visionTimer->Start();
	
	/* Camera */
#ifdef FEATURE_CAMERA
	{
		// Camera setup
		GetWatchdog().SetEnabled(false);
		Wait(5.0);
	#ifdef FEATURE_LCD
		lcd->PrintfLine(DS_LCD::kUser_Line1, "Done waiting for cam");
		lcd->UpdateLCD();
	#endif
	
		m_camera = &(AxisCamera::GetInstance());
		m_camera->WriteResolution(AxisCameraParams::kResolution_320x240);
		m_camera->WriteBrightness(0);
		m_camera->WriteCompression(75);
		m_camera->WriteMaxFPS(15);
		
		// Tell the operator we're just idling
	#ifdef FEATURE_LCD
		lcd->PrintfLine(DS_LCD::kUser_Line1, "Camera initialized");
		lcd->UpdateLCD();
		Wait(1.0);
	#endif
		GetWatchdog().SetEnabled(true);
	}
#endif

#ifdef FEATURE_LCD
	lcd->PrintfLine(DS_LCD::kUser_Line1, "Robot ready");
	lcd->UpdateLCD();
#endif
}

void BossRobot::Autonomous(void)
{
	GetWatchdog().SetEnabled(false);
//	m_drive->Drive(0.5, 0.0); 	// drive forwards half speed
//	Wait(2.0); 				//    for 2 seconds
//	m_drive->Drive(0.0, 0.0); 	// stop robot
}

void BossRobot::OperatorControl(void)
{
	// Main loop
	GetWatchdog().SetEnabled(true);
	GetWatchdog().Feed();
	
	while (IsOperatorControl())
	{
		GetWatchdog().SetEnabled(true);
		GetWatchdog().Feed();
		
		if (IsDisabled())
		{
			ChangeState(NULL);
		}
		else if (m_state == NULL)
		{
			ChangeState(new NormalState(this));
		}
		
		RunIteration();
		
		// Post-iteration clean up
		GetWatchdog().Feed();
		Wait(TELEOP_LOOP_LAG);				// wait for a motor update time
		GetWatchdog().Feed();
	}
}

void BossRobot::RunIteration(void)
{
	if (m_state != m_prevState)
	{
		// We must have changed states since the last iteration
		// 1. Exit out of "old" state
		if (m_prevState != NULL)
			m_prevState->Exit();
		GetWatchdog().Feed();
		// 2. Enter "new" state
		if (m_state != NULL)
			m_state->Enter();
		GetWatchdog().Feed();
		// 3. Record the state change as successful
		m_prevState = m_state;
	}
	
	// Do any pre-step logic
	PreStep();
	GetWatchdog().Feed();
	
	// Do what the state wants
	if (m_state != NULL)
	{
		m_state->Step();
	}
	GetWatchdog().Feed();
	
	// Do any post-step logic
	PostStep();
	GetWatchdog().Feed();
}

/** Perform any actions that need to be done before state updates */
void BossRobot::PreStep(void)
{
#ifdef FEATURE_COMPRESSOR
	m_compressor->Set(m_pressureSwitch->Get() ? Relay::kOff : Relay::kOn);
#endif
	
	m_elbowSwitch->Set(false);
}

/** Perform any actions that need to be done after state updates */
void BossRobot::PostStep(void)
{	
	// Send I/O data
	SendVisionData();
	GetWatchdog().Feed();
	SendIOPortData();
	GetWatchdog().Feed();
}

void BossRobot::ChangeState(State *st)
{
	// FIXME: This will leak memory
	m_state = st;
}

void BossRobot::SetDriveSystem(DriveSystem *d)
{
	if (m_driveSystem != d)
	{
		if (m_driveSystem != NULL)
			delete m_driveSystem;
		m_driveSystem = d;
	}
}

void BossRobot::SoftStop()
{
	m_leftMotor1->Set(0.0);
	m_leftMotor2->Set(0.0);
	m_rightMotor1->Set(0.0);
	m_rightMotor2->Set(0.0);
	m_shoulderMotor1->Set(0.0);
	m_shoulderMotor2->Set(0.0);
	m_kickerMotor->Set(0.0);
	m_kickerWinchRelay1->Set(Relay::kOff);
	m_kickerWinchRelay2->Set(Relay::kOff);
	m_intakeMotor->Set(0.0);
}

/** Send vision statistics to the operator interface */
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
			double gyroAngle = 0.0;
			
#ifdef FEATURE_GYRO
			gyroAngle = m_gyro->GetAngle();
			
			while (gyroAngle > 180.0)
				gyroAngle -= 360.0;
			while (gyroAngle < -180.0)
				gyroAngle += 360.0;
#endif
			
			dash.AddDouble(0.0); // Joystick X
			dash.AddDouble(gyroAngle); // angle
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

static UINT16 DIOHardware2Logical(UINT16 dio)
{
	UINT16 result = 0;
	int i;
	
	for (i = 0; i < 16; i++)
	{
		result |= ((dio & (1 << i)) >> i) << (16 - i - 1);
	}
	return result;
}

/** Send IO data to the operator interface */
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
					dash.AddU16(DIOHardware2Logical(DigitalModule::GetInstance(module)->GetDIO()));
					dash.AddU16(DIOHardware2Logical(DigitalModule::GetInstance(module)->GetDIODirection()));
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
		dash.AddU8((unsigned char)m_gearSwitch->Get() |
				   ((unsigned char)m_elbowSwitch->Get() << 1));
	}
	dash.FinalizeCluster();
	dash.Finalize();
}

START_ROBOT_CLASS(BossRobot);

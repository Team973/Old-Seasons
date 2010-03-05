//
//	Options.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/12/10.
//

// This file is used to collect all of the compile-time options for the robot.
// If you think that you need to have something that should be changed at compile
// time, then do a #define here.  If it needs to be disabled, comment it out.

#ifndef _BOSS_973_OPTIONS_H_
#define _BOSS_973_OPTIONS_H_

// Operator interface
#define FEATURE_CAMERA
#define FEATURE_LCD
#define FEATURE_IO_BOARD
#define DS_LCD MyDriverStationLCD

// Running
#define FEATURE_COMPRESSOR
#define FEATURE_UPPER_BOARD
#define FEATURE_GEAR_SWITCH
//#define FEATURE_GYRO
//#define FEATURE_DRIVE_ENCODERS
#define FEATURE_DRIVE_VICTORS

#endif

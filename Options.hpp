//
//	Options.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/12/10.
//

/**
 *	@file Options.hpp
 *
 *	This file is used to collect all of the compile-time options for the robot.
 *
 *	Before putting anything new into this file, think carefully.  Could this
 *	actually be a run-time option?  Is it conceivable that the option will
 *	need to be changed on a fairly regular or at least mildly painless basis?
 *	If so, then make a new configuration value and use the ConfigParser API.
 *
 *	If you think that you need to have something that can only be changed at
 *	compile time, then do a <code>\#define</code> here.	If the option needs to
 *	be disabled, comment it out; don't remove it from the file.
 *
 *	The options currently defined are:
 *		- FEATURE_CAMERA
 *		- FEATURE_LCD
 *		- FEATURE_IO_BOARD
 *		- DS_LCD
 *		- FEATURE_COMPRESSOR
 *		- FEATURE_UPPER_BOARD
 *		- FEATURE_GEAR_SWITCH
 *		- FEATURE_GYRO
 *		- FEATURE_DRIVE_ENCODERS
 *		- FEATURE_DRIVE_VICTORS
 */

#ifndef _BOSS_973_OPTIONS_H_
#define _BOSS_973_OPTIONS_H_

// Operator interface
#define FEATURE_CAMERA 				/**< Camera is enabled */
#define FEATURE_LCD					/**< Printing to the driver station is enabled */
#define FEATURE_IO_BOARD			/**< Operator interface has the Cypress module */

/**
 *	An alias for the DriverStationLCD class.
 *
 *	The reason this exists merits a bit of discussion; it's a bit of a kludge.
 *	In version 4.1 of the WPILib (2010-02-03), the kLineLength symbol from the
 *	DriverStationLCD class was not properly linked, rendering any code that
 *	used the class fail.  For a while, Ross just kept the FEATURE_LCD option
 *	disabled.  However, the overwhelming usefulness of printing to the screen
 *	compelled Ross to grab the DriverStationLCD code directly from the WPILib
 *	repository, add a sufficiently unique prefix, fix the bug, and commit it to
 *	the Boss source tree.
 *
 *	We kept around the DS_LCD alias in the hope that one day WPILib will fix
 *	this.  When that day comes, we may change back DS_LCD to use the WPILib
 *	implementation and there will be great peace in the land.
 */
#define DS_LCD MyDriverStationLCD

// Running
#define FEATURE_COMPRESSOR			/**< Run the compressor */
#define FEATURE_UPPER_BOARD			/**< Use the upper board sidecar */
#define FEATURE_GEAR_SWITCH			/**< Allow gear switching */
//#define FEATURE_GYRO				/**< Enable deadheading compensation based on gyro data */
#define FEATURE_DRIVE_ENCODERS	/**< Enable inert compensation based on drive encoders */
#define FEATURE_DRIVE_VICTORS		/**< Drive uses Victors or Jaguars (default is Jaguars) */

#endif

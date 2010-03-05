/**
 *	@file ControlBoard.hpp
 *	Header for the ControlBoard class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "WPILib.h"
#include "Options.hpp"

#ifndef _BOSS_973_CONTROLBOARD_H_
#define _BOSS_973_CONTROLBOARD_H_

/**
 *	Abstraction for the control board.
 *
 *	Yes, you can go and use WPILib's APIs.  And yes, this one is mostly just a
 *	wrapper around the WPILib functions.  However, there are some shortcuts and
 *	simplifications that this class will give you.  For instance, dealing with
 *	the Cypress I/O is much easier here.
 */
class ControlBoard
{
protected:
	Joystick m_stick1, m_stick2, m_stick3;
	
	ControlBoard();
public:
	enum
	{
		kLightOff = 0,		/**< LED off state */
		kLightRed = 1,		/**< LED red light only */
		kLightGreen = 2,	/**< LED green light only */
		kLightYellow = 3,	/**< LED both lights */
	};
	
	/**
	 *	Acquire the shared control board instance.
	 *
	 *	You never need to create a control board instance yourself.  There can
	 *	be only one, Highlander.
	 *
	 *	@return The shared control board
	 */
    static ControlBoard &GetInstance();
    
    /**
     *	Obtain a joystick.
     *
     *	@param index
     *		The 1-based index of the joystick.
     *	@return The requested joystick
     */
	Joystick &GetJoystick(int index);
	
	/**
	 *	Get the state of a button wired into the Cypress.
	 *
	 *	@param buttonNum
	 *		The 1-based channel number of the button.  See the wiring diagram
	 *		that Dustin made for details.
	 *	@return The boolean value of the button
	 */
	bool GetButton(UINT16 buttonNum);
	
	/**
	 *	Change the color of an LED wired into the Cypress.
	 *
	 *	@param greenLight
	 *		The 1-based channel number of the green LED.  See the wiring diagram
	 *		that Dustin made for details.
	 *	@param redLight
	 *		The 1-based channel number of the red LED.  See the wiring diagram
	 *		that Dustin made for details.
	 *	@param state
	 *		One of #kLightOff, #kLightRed, #kLightGreen, or #kLightYellow.
	 */
	void SetMultiLight(UINT16 greenLight, UINT16 redLight, short state);
	
	/**
	 *	Change the color of an LED wired into the Cypress.
	 *
	 *	This infers the red light by adding two to the given light number (all
	 *	of Dustin's LED wirings make the red light two channels away).
	 *
	 *	@param lightNum
	 *		The 1-based channel number of the green LED.  See the wiring diagram
	 *		that Dustin made for details.
	 *	@param state
	 *		One of #kLightOff, #kLightRed, #kLightGreen, or #kLightYellow.
	 *	@see SetMultiLight(UINT16, UINT16, short)
	 */
	void SetMultiLight(UINT16 lightNum, short state);
};

#endif

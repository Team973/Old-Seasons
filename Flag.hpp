/**
 *	@file Flag.hpp
 *	Header for the Flag class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/26/10.
 */

#ifndef _BOSS_973_FLAG_H_
#define _BOSS_973_FLAG_H_

/**
 *	A convenient way of detecting state changes.
 *
 *	A flag is mostly a boolean, but it also stores the "previous" state of that
 *	boolean.  This way, we can easily detect changes in the boolean.
 *
 *	Some examples of where this could be useful:
 *		- A trigger on a joystick
 *		- Performing an action only when a sensor changes
 */
class Flag
{
protected:
	bool m_value, m_prev;
public:
	/** Create a flag with a default of false. */
	Flag();
	/** Create a flag with a given value. */
	Flag(bool initialValue);
	
	/**
	 *	Obtain the current value of the flag.
	 *
	 *	@return The flag's value
	 */
	bool Get();
	
	/**
	 *	Update the current value of the flag.
	 *
	 *	This may or may not cause a "trigger" event.
	 *
	 *	@param v
	 *		The new value of the flag
	 */
	void Set(bool v);
	
	/**
	 *	Check to see whether the value has changed from the previous update.
	 *
	 *	This does not clear the trigger event, so you can call this multiple
	 *	times and it will return the same result (until you call #ClearTrigger).
	 *	
	 *	@return Whether the value has changed
	 *	@see GetTriggered
	 */
	bool CheckTriggered();
	
	/**
	 *	Check to see whether the value has changed from the previous update and
	 *	is true.
	 *
	 *	This does not clear the trigger event, so you can call this multiple
	 *	times and it will return the same result (until you call #ClearTrigger).
	 *
	 *	@return Whether the value has changed to true
	 *	@see GetTriggeredOn
	 */
	bool CheckTriggeredOn();
	
	/**
	 *	Check to see whether the value has changed from the previous update and
	 *	is false.
	 *
	 *	This does not clear the trigger event, so you can call this multiple
	 *	times and it will return the same result (until you call #ClearTrigger).
	 *
	 *	@return Whether the value has changed to false
	 *	@see GetTriggeredOff
	 */
	bool CheckTriggeredOff();
	
	/**
	 *	Clear the triggered event state.
	 *
	 *	Any subsequent calls to #CheckTriggered, #GetTriggered, or any related
	 *	functions will return false.
	 */
	void ClearTrigger();
	
	/**
	 *	Check to see whether the value has changed from the previous update.
	 *
	 *	This clears the trigger event, so if you call this multiple times, the
	 *	first time may return true, but the rest will be false.
	 *	
	 *	@return Whether the value has changed
	 *	@see ClearTrigger, CheckTriggered
	 */
	bool GetTriggered();
	
	/**
	 *	Check to see whether the value has changed from the previous update and
	 *	is true.
	 *
	 *	This clears the trigger event, so if you call this multiple times, the
	 *	first time may return true, but the rest will be false.
	 *	
	 *	@return Whether the value has changed
	 *	@see ClearTrigger, CheckTriggeredOn
	 */
	bool GetTriggeredOn();
	
	/**
	 *	Check to see whether the value has changed from the previous update and
	 *	is false.
	 *
	 *	This clears the trigger event, so if you call this multiple times, the
	 *	first time may return true, but the rest will be false.
	 *	
	 *	@return Whether the value has changed
	 *	@see ClearTrigger, CheckTriggeredOff
	 */
	bool GetTriggeredOff();
};

#endif

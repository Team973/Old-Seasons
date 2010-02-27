//
//	Flag.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/26/10.
//

#ifndef _BOSS_973_FLAG_H_
#define _BOSS_973_FLAG_H_

class Flag
{
protected:
	bool m_value, m_prev;
public:
	Flag();
	Flag(bool);
	
	bool Get();
	void Set(bool);
	
	bool CheckTriggered();
	bool CheckTriggeredOn();
	bool CheckTriggeredOff();
	
	bool GetTriggered();
	bool GetTriggeredOn();
	bool GetTriggeredOff();
	void ClearTrigger();
};

#endif

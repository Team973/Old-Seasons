//
//	ConfigParser.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/15/10.
//

#include <map>
#include <string>

#ifndef _BOSS_973_CONFIGPARSER_H_
#define _BOSS_973_CONFIGPARSER_H_

class ConfigParser
{
protected:
	std::map<std::string, std::string> m_values;
public:
	ConfigParser();
	~ConfigParser();
	
	bool Read(std::string filename);
	bool Write(std::string filename);
	
	bool Has(std::string key) const;
	std::string Get(std::string key) const;
	int GetInt(std::string key) const;
	double GetDouble(std::string key) const;
	
	void SetDefault(std::string key, std::string value);
	void SetDefault(std::string key, int value);
	void SetDefault(std::string key, double value);
	void Set(std::string key, std::string value);
	void Set(std::string key, int value);
	void Set(std::string key, double value);
};

#endif

//
//	ConfigParser.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
//

#include <map>
#include <string>

#ifndef _BOSS_973_CONFIGPARSER_H_
#define _BOSS_973_CONFIGPARSER_H_

class ConfigParser
{
protected:
	std::map<std::string, std::string> m_values;
	
	void ParseLine(const std::string &line);
public:
	ConfigParser();
	~ConfigParser();
	void Clear();
	
	bool Read(std::string filename);
	bool Write(std::string filename, std::string description="This is a config file.");
	
	bool Has(std::string key) const;
	std::string Get(std::string key) const;
	int GetInt(std::string key) const;
	double GetDouble(std::string key) const;
	
	std::string SetDefault(std::string key, std::string value);
	int SetDefault(std::string key, int value);
	double SetDefault(std::string key, double value);
	void Set(std::string key, std::string value);
	void Set(std::string key, int value);
	void Set(std::string key, double value);
};

#endif

//
//	ConfigParser.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/15/10.
//

#include "ConfigParser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

bool ConfigParser::Read(std::string filename)
{
	std::ifstream f;
	std::string key, value;
	
	f.open(filename.c_str());
	
	while (f.good())
	{
		getline(f, key, '=');
		getline(f, value);
		Set(key, value);
	}
}

bool ConfigParser::Write(std::string filename)
{
	std::ofstream f;
	std::map<std::string, std::string>::const_iterator i;
	
	f.open(filename.c_str());
	
	for (i = m_values.begin(); i != m_values.end(); i++)
	{
		f << (i->first);
		f << "=";
		f << (i->second);
		f << "\n";
	}
}

bool ConfigParser::Has(std::string key) const
{
	std::map<std::string, std::string>::const_iterator i;
	
	i = m_values.find(key);
	return i != m_values.end();
}

std::string ConfigParser::Get(std::string key) const
{
	std::map<std::string, std::string>::const_iterator i;
	
	i = m_values.find(key);
	if (i != m_values.end())
		return i->second;
	else
		return "";
}

int ConfigParser::GetInt(std::string key) const
{
	std::istringstream st;
	int result = 0;
	
	st.str(Get(key));
	st >> result;
	return result;
}

double ConfigParser::GetDouble(std::string key) const
{
	std::istringstream st;
	double result = 0.0;
	
	st.str(Get(key));
	st >> result;
	return result;
}

void ConfigParser::SetDefault(std::string key, std::string value)
{
	if (m_values.find(key) == m_values.end())
	{
		m_values[key] = value;
	}
}

void ConfigParser::SetDefault(std::string key, int value)
{
	std::ostringstream st;
	
	st << value;
	SetDefault(key, st.str());
}

void ConfigParser::SetDefault(std::string key, double value)
{
	std::ostringstream st;
	
	st << value;
	SetDefault(key, st.str());
}

void ConfigParser::Set(std::string key, std::string value)
{
	m_values[key] = value;
}

void ConfigParser::Set(std::string key, int value)
{
	std::ostringstream st;
	
	st << value;
	Set(key, st.str());
}

void ConfigParser::Set(std::string key, double value)
{
	std::ostringstream st;
	
	st << value;
	Set(key, st.str());
}

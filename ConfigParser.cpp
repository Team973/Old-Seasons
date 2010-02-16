//
//	ConfigParser.cpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
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
	std::string line, key, value;
	std::string::iterator i;
	
	f.open(filename.c_str());
	if (f.fail())
		return false;
	
	while (f.good())
	{
		// Read line
		getline(f, line);
		// Parse line
		for (i = line.begin(); i != line.end(); i++)
		{
			if (*i == '=')
			{
				key = std::string(line.begin(), i);
				value = std::string(i + 1, line.end());
				break;
			}
		}
		
		if (i != line.end())
			Set(key, value);
	}
	
	return !f.bad();
}

bool ConfigParser::Write(std::string filename)
{
	std::ofstream f;
	std::map<std::string, std::string>::const_iterator i;
	
	f.open(filename.c_str());
	if (f.fail())
		return false;
	
	f << "# ";
	
	for (i = m_values.begin(); i != m_values.end() && f.good(); i++)
	{
		f << (i->first);
		f << "=";
		f << (i->second);
		f << "\n";
	}
	
	return !f.fail();
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

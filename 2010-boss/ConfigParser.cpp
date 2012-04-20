/**
 *	@file ConfigParser.cpp
 *	Implementation of the ConfigParser class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/16/10.
 */

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

void ConfigParser::Clear()
{
	m_values.clear();
}

bool ConfigParser::Read(std::string filename)
{
	std::ifstream f;
	std::string line;
	
	f.open(filename.c_str());
	if (f.fail())
		return false;
	
	while (f.good())
	{
		getline(f, line);
		ParseLine(line);
	}
	
	return !f.bad();
}

void ConfigParser::ParseLine(const std::string &line)
{
    std::string key, value;
	std::string::const_iterator i;
	
    if (line[0] == '#')
    {
        // This is a comment; move along.
        return;
    }
    
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

bool ConfigParser::Write(std::string filename, std::string description)
{
	std::ofstream f;
	std::map<std::string, std::string>::const_iterator i;
	
	f.open(filename.c_str());
	if (f.fail())
		return false;
	
	f << "# " + description << "\n";
	f << "# Each line in this file must be in the form: name=value\n";
	f << "# There must not be any spaces around the equals sign.\n";
	f << "# Any line that starts with # is ignored and treated as a comment.\n";
	f << "\n";
	
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

std::string ConfigParser::SetDefault(std::string key, std::string value)
{
	std::map<std::string, std::string>::iterator i;
	
	i = m_values.find(key);
	
	if (i == m_values.end())
	{
		m_values[key] = value;
		return value;
	}
	else
	{
		return i->second;
	}
}

int ConfigParser::SetDefault(std::string key, int value)
{
	std::ostringstream ost;
	std::istringstream ist;
	int result = 0;
	
	ost << value;
	ist.str(SetDefault(key, ost.str()));
	ist >> result;
	return result;
}

double ConfigParser::SetDefault(std::string key, double value)
{
	std::ostringstream ost;
	std::istringstream ist;
	double result = 0.0;
	
	ost << value;
	ist.str(SetDefault(key, ost.str()));
	ist >> result;
	return result;
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

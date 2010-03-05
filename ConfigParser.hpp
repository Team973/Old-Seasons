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

/**
 *	Configuration file parser.
 *
 *	The configuration file parser acts as a simple key-based database.	Each
 *	"key" (a name) has a "value" (some data) associated with it.  You can access
 *	any value by asking for its key, and you can set new values into the keys.
 *
 *	You can read the values from a file and write them back to a file.	However,
 *	this process will not be done automatically, because file I/O is a slow
 *	process.
 */
class ConfigParser
{
protected:
	std::map<std::string, std::string> m_values;
	
	void ParseLine(const std::string &line);
public:
	/**
	 *	Create a configuration parser.
	 *
	 *	There will be no values in the parser by default.
	 */
	ConfigParser();
	~ConfigParser();
	
	/** Delete all values from the configuration parser. */
	void Clear();
	
	/**
	 *	Load configuration data from the filesystem.
	 *
	 *	Any values loaded will overwrite values that were already in the
	 *	configuration parser.
	 *
	 *	@param filename
	 *		The path of the file to load from
	 *	@return Whether the load was successful
	 */
	bool Read(std::string filename);
	
	/**
	 *	Save configuration data to the filesystem.
	 *
	 *	The file will be completely overwritten with the values in the parser.
	 *
	 *	@param filename
	 *		The path of the file to save to
	 *	@param description
	 *		An optional comment at the top of the written configuration file
	 *	@return Whether the save was successful
	 */
	bool Write(std::string filename, std::string description="This is a config file.");
	
	/**
	 *	Check whether a key is present in the parser.
	 *
	 *	@param key
	 *		The key to check for
	 *	@return Whether the key exists
	 */
	bool Has(std::string key) const;
	
	/**
	 *	Retrieve the value of a key from the parser.
	 *
	 *	@param key
	 *		The key to retrieve from
	 *	@return The value for the key, or an empty string if the key was not
	 *			found.
	 */
	std::string Get(std::string key) const;
	
	/**
	 *	Retrieve the integer value of a key from the parser.
	 *
	 *	@param key
	 *		The key to retrieve from
	 *	@return The value for the key, or zero if the key was not found.
	 */
	int GetInt(std::string key) const;
	
	/**
	 *	Retrieve the floating-point value of a key from the parser.
	 *
	 *	@param key
	 *		The key to retrieve from
	 *	@return The value for the key, or zero if the key was not found.
	 */
	double GetDouble(std::string key) const;
	
	/**
	 *	Change the value of a key in the parser.
	 *
	 *	@param key
	 *		The key whose value should be changed
	 *	@param value
	 *		The new value to store
	 */
	void Set(std::string key, std::string value);
	
	/**
	 *	Change the value of a key in the parser.
	 *
	 *	@param key
	 *		The key whose value should be changed
	 *	@param value
	 *		The new value to store
	 */
	void Set(std::string key, int value);
	
	/**
	 *	Change the value of a key in the parser.
	 *
	 *	@param key
	 *		The key whose value should be changed
	 *	@param value
	 *		The new value to store
	 */
	void Set(std::string key, double value);
	
	/**
	 *	Set a default for the value.
	 *
	 *	If there is no value associated with the given key, then the given value
	 *	will be stored under the given key.  If the given key is already present
	 *	in the configuration parser, then nothing happens.
	 *
	 *	@param key
	 *		The key to check
	 *	@param value
	 *		The default to store (if necessary)
	 *	@return The default or the already stored value
	 */
	std::string SetDefault(std::string key, std::string value);
	
	/**
	 *	Set an integer default for the value.
	 *
	 *	If there is no value associated with the given key, then the given value
	 *	will be stored under the given key.  If the given key is already present
	 *	in the configuration parser, then nothing happens.
	 *
	 *	@param key
	 *		The key to check
	 *	@param value
	 *		The default to store (if necessary)
	 *	@return The default or the already stored value
	 */
	int SetDefault(std::string key, int value);
	
	/**
	 *	Set a floating-point default for the value.
	 *
	 *	If there is no value associated with the given key, then the given value
	 *	will be stored under the given key.  If the given key is already present
	 *	in the configuration parser, then nothing happens.
	 *
	 *	@param key
	 *		The key to check
	 *	@param value
	 *		The default to store (if necessary)
	 *	@return The default or the already stored value
	 */
	double SetDefault(std::string key, double value);
};

#endif

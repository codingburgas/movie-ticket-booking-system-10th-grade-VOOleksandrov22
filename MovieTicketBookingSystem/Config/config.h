#pragma once

#include <iostream>


class Config {
public:
	// DATABASE SETTINGS

	std::string url = "url to mysql db";
	std::string username = "username to log in with";
	std::string password = "password to lg in with";
	std::string schema = "db schema to use";
	bool debugMode = true;

	// DATABASE SETTINGS
	Config() {}
};
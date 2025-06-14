#pragma once

#include <iostream>
#include <map>


class Config {
public:

	std::string pathToCache = "cache/cache.json";

	// DATABASE SETTINGS

	std::string url = "mysql://localhost:3306/";
	std::string username = "root";
	std::string password = "12345678";
	std::string schema = "movieticketbookingsystem";
	bool debugMode = true;

	// DATABASE SETTINGS


	// EMAIL SETTINGS

	std::string projectEmail = "movieticketbookingsystemproj@gmail.com";
	std::string projectEmailPassword = "vfuh ojjm beza vfms";

	// EMAIL SETTINGS


	std::map<char, std::string> genders = {
		{'M', "Male"},
		{'F', "Female"},
		{'O', "Other"},
		{'P', "Prefer not to say"}
	};


	Config() {}

	const char getShortenedGender(const std::string& gender) const;

};
#pragma once

#include <iostream>
#include <map>
#include <array>


class Config {
public:

	const std::string pathToCache = "cache/cache.json";

	// DATABASE SETTINGS

	const std::string url = "mysql://localhost:3306/";
	const std::string username = "root";
	const std::string password = "12345678";
	const std::string schema = "movieticketbookingsystem";
	const bool debugMode = true;

	// DATABASE SETTINGS


	// EMAIL SETTINGS

	const std::string projectEmail = "movieticketbookingsystemproj@gmail.com";
	const std::string projectEmailPassword = "vfuh ojjm beza vfms";

	// EMAIL SETTINGS


	std::map<char, std::string> genders = {
		{'M', "Male"},
		{'F', "Female"},
		{'O', "Other"},
		{'P', "Prefer not to say"}
	};


	Config() {}

	const char getShortenedGender(const std::string& gender) const;

	const std::array<int, 2> customMenuLayoutItemSize = { 10, 6 };

};
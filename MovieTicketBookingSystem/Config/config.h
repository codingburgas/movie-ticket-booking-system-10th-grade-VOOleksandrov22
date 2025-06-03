#pragma once

#include <iostream>


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

	Config() {}
};
#pragma once

#include <iostream>
#include <map>
#include "config.h"
#include "../Database/dbWrapper.h"
#include "../Menu/menu.h"

class Session;


class App {
private:

public:
	const Config* config = new Config();

	const DbWrapper* db;

	Menu* menu;

	Session* currentSession;

	App();

	void loginBySavedSession();

	void auth(std::string message="");

	void login();

	void signup();

	void logout();

	void mainLoop();
	
};
#pragma once

#include <iostream>
#include <map>
#include "../Config/config.h"
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

	void chooseCityMenu();

	void chooseCinemaMenu(const std::string& city);

	void chooseMovieMenu(const unsigned int& cinemaId);

	void chooseSeatMenu(Row& session);

	void bookTicket(Row& movie, const json& seatData);


	std::vector<std::string> getCities();

	
};
#pragma once

#include <iostream>
#include <functional>
#include <map>
#include "../Config/config.h"
#include "../Database/database.h"
#include "../Menu/menu.h"
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

class User;

class Session;


class App {
private:
	std::function<std::string(json&)> regular;
	std::function<std::string(json&)> highlight;
	std::function<bool(json&)> skipCheck;
public:

	

	const Config* config = new Config();

	DB::Database* db;

	Menu* menu;

	Session* currentSession;

	App();

	void defineHelperMethods();

	void loginBySavedSession();

	void auth(std::string message="");

	void login();

	void signup();

	void logout();

	void mainLoop();

	void chooseCityMenu();

	void chooseCinemaMenu(const std::string& city);

	void chooseMovieMenu(const unsigned int& cinemaId);


	void bookTicket(Row& movie);


	std::vector<std::string> getCities();

	void profilePage();

	void updateProfileDataPage(User& user);

	void depositPage();
	
};
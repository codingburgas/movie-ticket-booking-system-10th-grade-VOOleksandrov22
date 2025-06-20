#pragma once

#include <iostream>
#include <map>
#include <conio.h>
#include <vector>
#include <cstdlib>
#include <functional>
#include <format>
#include "app.h"
#include "../Config/config.h"
#include "../Form/form.h"
#include "../Validation/validation.h"
#include "../menu/menu.h"

//#include <cppconn/resultset.h>

#include "../Database/database.h"
#include "../Colors/colors.h"

#include <string>

using json = nlohmann::json;

using RedirectFunction = std::function<void()>;

class User;

class Session;


class App {
private:
	std::function<std::string(json&, const User&)> regular;
	std::function<std::string(json&, const User&)> highlight;
	std::function<bool(json&, const User&)> skipCheck;
public:

	std::map<std::string, std::string> tempValues; // Temporary values for forms and other purposes

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

	void depositPage(User& user);

	void printTransactions(const User& user);
	
};
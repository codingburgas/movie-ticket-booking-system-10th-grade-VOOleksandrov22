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
#include "../Redirect/redirect.h"

//#include <cppconn/resultset.h>

#include "../Database/database.h"
#include "../Colors/colors.h"

#include <string>

using json = nlohmann::json;

using RedirectFunction = std::function<void()>;

class User;

class Session;


const std::string passwordInstructions = R"(
- 8 to 64 characters.
- At least one uppercase letter, one lowercase, one digit, and one special character (e.g., !@#$%^&*).
)";


enum class Action;


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

	void auth();

	void login();

	void signup();

	void logout();

	void mainMenu();

	void bookTicket();

	std::string chooseCityMenu();

	unsigned long chooseCinemaMenu(const std::string& city);

	unsigned long chooseHallMenu(const unsigned long& cinemaId);

	unsigned long chooseMovieFromAll();

	Row chooseMovieMenu(const unsigned long& cinemaId);


	void bookTicket(const Row& movie);


	std::vector<std::string> getCities();

	void profilePage();

	void updateProfileDataPage(User& user);

	void depositPage(User& user, const std::function<void()>& redirectTo);

	void printTransactions(const User& user);

	void changePassword(const User& user);

	void forgotPassword();

	void adminPage();

	// --- Cinema CRUD Functions ---
	void createCinema();
	void updateCinema();
	void deleteCinema();

	// --- Hall CRUD Functions ---
	void createHall();

	void updateHall();
	json createSeatLayoutMenu();

	void deleteHall();

	//// --- Movie CRUD Functions ---
	void createMovie();
	void updateMovie();
	void deleteMovie();

	//// --- MovieSession CRUD Functions ---
	void createMovieSession();
	
};


std::string addLineUnderBlockIfHighlighted(const std::string& block, json& data, const App* app);
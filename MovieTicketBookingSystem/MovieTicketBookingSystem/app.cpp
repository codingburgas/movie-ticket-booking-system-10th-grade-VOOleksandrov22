#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <functional>
#include <format>
#include "app.h"
#include "config.h"

//#include <cppconn/resultset.h>
#include "../nlohmann/json.hpp"
#include "../Utils/utils.h";
#include "../../db_cpp/database.h"
#include "../Date/date.h"
#include "session.h"
#include "../Colors/colors.h"

#include "../../password_input/PasswordInput/include/options.h"
#include "../../password_input/PasswordInput/include/passwordInput.h"

using RedirectFunction = std::function<void()>;


using json = nlohmann::json;

App::App() 
	: 
	db(new DbWrapper(config->url, config->username, config->password, config->schema, config->debugMode)),
	menu(new Menu())
{
	loginBySavedSession();
	mainLoop();
}






void App::mainLoop() {
	bool running = true;

	auto user = currentSession->getUser();

	std::map<std::string, RedirectFunction> redirects = {
		{"Buy a ticket", [this]() -> void { this->chooseCityMenu(); }},
		{"Log out", [this]() -> void { this->logout(); } },
		{"Exit",[]() -> void { exit(0); }},
	};

	std::vector<std::string> startingPageOptions = {
		"Buy a ticket",
		"Log out",
		"Exit"
	};

	if (currentSession->getUser().getIsAdmin()) {
		auto elementBeforeLast = startingPageOptions.end() - 2;
		startingPageOptions.insert(elementBeforeLast, "Admin Options");
	}

	while (running) {
		size_t choice = menu->getChoice(startingPageOptions, "");
		if (redirects.find(startingPageOptions[choice]) != redirects.end()) {
			redirects[startingPageOptions[choice]]();
		}
		else std::cerr << "Unexpected error occured!" << std::endl;
	}
}


void App::chooseCityMenu() {
	system("cls");

	auto cities = getCities();

	std::vector<std::string> menuOptions = {};

	for (auto& city : cities) {
		std::string cinemaStatsQuery = "SELECT COUNT(*), AVG(rating) FROM Cinema WHERE city = '" + city + "';";
		auto res = db->db->execute(cinemaStatsQuery);

		std::string option = city + " ";
		if (res->next()) {
			option += res->getString(1) + " cinema(s), " + res->getString(2).substr(0, 3) + " rating on average";
		}

		menuOptions.push_back(option);
	}
	menuOptions.push_back("<< Back");


	size_t choice = menu->getChoice(menuOptions, (menuOptions.size() != 1) ? "Choose city:" : "Sorry, no cities found :(");
	if (choice == menuOptions.size() - 1) return;

	std::string cityChosen = cities[choice];

	chooseCinemaMenu(cityChosen);

}

void App::chooseCinemaMenu(const std::string& city) {
	std::string cinemaQuery = std::format("select * from Cinema where city = '{}';", city);
	auto cinemas = DB::resultSetToVector(db->db->execute(cinemaQuery));

	std::vector<std::string> menuOptions = {};

	for (auto& cinema : cinemas) {
		std::string vipHallCountQuery = std::format("select COUNT(*) from Hall where cinema_id = {} AND isVIP = TRUE;", cinema["id"]);
		std::string nonVipHallCountQuery = std::format("select COUNT(*) from Hall where cinema_id = {} AND isVIP = FALSE;", cinema["id"]);

		unsigned int vipCount = 0, nonVipCount = 0;

		auto vips = db->db->execute(vipHallCountQuery);
		if (vips->next()) vipCount = vips->getInt(1);

		auto nonVips = db->db->execute(nonVipHallCountQuery);
		if (nonVips->next()) nonVipCount = nonVips->getInt(1);

		std::string option = std::format("{} : {}, {} hall(s)({} VIP)", cinema["name"], cinema["rating"], vipCount + nonVipCount, vipCount);
		menuOptions.push_back(option);
	}

	menuOptions.push_back("<< Back");

	size_t choice = menu->getChoice(menuOptions, (menuOptions.size() != 1) ? std::format("Choose a cinema in {}", city) : "Sorry, no cinemas available :(");

	if (choice == menuOptions.size() - 1) return;
	
	chooseMovieMenu(std::stoul(cinemas[choice]["id"]));
	

}

void App::chooseMovieMenu(const unsigned int& cinemaId) {
	std::string moviesQuery = std::format("select * from Movie where cinema_id = {} AND startsAt > NOW();", cinemaId);

	auto movies = DB::resultSetToVector(db->db->execute(moviesQuery));

	std::vector<std::string> menuOptions = {};

	for (auto& movie : movies) {
		std::string option = std::format("{}(IMDb {})\n{}\n\nStarts at: {}\n", movie["title"], movie["rating"], movie["description"], movie["startsAt"]);

		menuOptions.push_back(option);
	}

	menuOptions.push_back("<< Back");

	size_t choice = menu->getChoice(menuOptions, (menuOptions.size() != 1) ? "Choose a movie:" : "Sorry, no movies available :(");

	if (choice == menuOptions.size() - 1) return;

	chooseSeatMenu(std::stoul(movies[choice]["id"]));
	

	
}




std::string regular(json& data) {
	std::string text = data["data"]["text"].get<std::string>();
	return std::format(
		"╭───────╮\n"
		"|       |\n"
		"|{:^7}|\n"
		"|       |\n"
		"╰───────╯",
		text
	);
}

std::string highlight(json& data) {
	std::string text = data["data"]["text"].get<std::string>();
	return std::format(
		"{}╭───────╮{}\n"
		"{}|       |{}\n"
		"{}|{:^7}|{}\n"
		"{}|       |{}\n"
		"{}╰───────╯{}",
		RED, RESET, RED, RESET, RED, text, RESET, RED, RESET, RED, RESET
	);
}

void App::chooseSeatMenu(const unsigned int& movieId) {
	std::string movieQuery = std::format("select * from Movie where id = {}", movieId);

	auto movie = DB::resultSetToVector(db->db->execute(movieQuery))[0];

	json seats = json::parse(movie["seats"]);

	int itemSize[2] = { 9, 5 };
	std::pair<size_t, size_t> seatChosen = menu->getChoice(seats,
		highlight,
		regular,
		itemSize,
		std::format("Choose a seat for a \"{}\" at {}!", movie["title"], movie["startsAt"])
	);

	bookTicket(&movie, &seats[seatChosen.first][seatChosen.second]);
	std::cout << std::format("You have chosen ({}, {})", seatChosen.first, seatChosen.second);
}


void App::bookTicket(Row& movie, const json& seatData) {

}
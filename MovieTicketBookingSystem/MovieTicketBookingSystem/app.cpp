#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <functional>
#include <format>
#include "app.h"
#include "../Config/config.h"

//#include <cppconn/resultset.h>
#include "../nlohmann/json.hpp"
#include "../../db_cpp/database.h"
#include "../Date/date.h"
#include "session.h"
#include "../Colors/colors.h"

#include "../../password_input/PasswordInput/include/options.h"
#include "../../password_input/PasswordInput/include/passwordInput.h"

#include <string>

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

	std::vector<std::pair<std::string, RedirectFunction>> redirects = {
		{"Buy a ticket", [this]() -> void { this->chooseCityMenu(); }},
		{"Log out", [this]() -> void { this->logout(); } },
		{"Exit",[]() -> void {
			system("cls");
			std::cout << "\nTill next time! (^///^)\n";
			exit(0);
		}},
	};

	/*std::vector<std::string> startingPageOptions = {
		"Buy a ticket",
		"Log out",
		"Exit"
	};*/

	if (currentSession->getUser().getIsAdmin()) {
		auto elementBeforeLast = redirects.end() - 2;
		redirects.insert(elementBeforeLast,
			{ "Admin panel", [this]() -> void { std::cout << "TO DO: make admin page"; } }
		);
	}

	while (running) {
		std::vector<std::string> actions = {};
		for (const auto& redirect : redirects) {
			actions.push_back(redirect.first);
		}
		size_t choice = menu->getChoice(actions, "Actions are:");

		redirects[choice].second();
	}
}


void App::chooseCityMenu() {
	system("cls");

	auto cities = getCities();

	std::vector<std::string> menuOptions = {};

	for (auto& city : cities) {
		std::string cinemaStatsQuery = std::format("SELECT COUNT(*), AVG(rating) FROM Cinema WHERE city = '{}';", city);
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
	std::cout << cinemaId;
	std::string sessionsQuery = std::format(
		R"(select
		ms.id,
		ms.startsAt,
		m.title,
		m.rating,
		m.description,
		m.duration,
		h.id as hall_id
		from hall h
		join moviesession ms on ms.hall_id = h.id and ms.startsAt > now()
		join movie m on ms.movie_id = m.id
		where h.cinema_id = {};)",
		cinemaId
	);

	auto sessions = DB::resultSetToVector(db->db->execute(sessionsQuery));

	std::vector<std::string> menuOptions = {};

	for (auto& session : sessions) {
		std::string option = std::format(
			"{}(IMDb {})\n{}\n\nStarts at: {}\nDuration: {}",
			session["title"],        
			session["rating"],       
			session["description"],  
			session["startsAt"],     
			session["duration"]      
		);

		menuOptions.push_back(option);
	}

	menuOptions.push_back("<< Back");

	size_t choice = menu->getChoice(menuOptions, (menuOptions.size() != 1) ? "Choose a movie:" : "Sorry, no movies available :(");

	if (choice == menuOptions.size() - 1) return;

	chooseSeatMenu(sessions[choice]);
	

	
}





// FUNCTION WHICH CREATES REGULAR MENU ITEM ON GIVEN DATA
std::string regular(json& data) {
	if (data["data"]["isBlank"].get<bool>()) {
		return
			"            \n"
			"            \n"
			"            \n"
			"            \n"
			"            ";
	}

	std::string text = data["data"]["text"].get<std::string>();
	return std::format(
		"╭────────╮\n"
		"|{}{:^8}{}|\n"
		"|{:^8}|\n"
		"|{}|\n"
		"╰────────╯",
		GREEN,
		Utils::String::toString(data["data"]["price"].get<double>(), 2) + "$",
		RESET,
		text,
		data["data"]["isVIP"].get<bool>()
		? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), RESET)
		: std::format("{:^8}", "")
	);
}

// FUNCTION WHICH CREATES HIGHLIGHTED MENU ITEM ON GIVEN DATA
std::string highlight(json& data) {
	if (data["data"]["isBlank"].get<bool>()) {
		return
			"            \n"
			"            \n"
			"            \n"
			"            \n"
			"            ";
	}

	std::string text = data["data"]["text"].get<std::string>();
	return std::format(
		"{}╭────────╮{}\n"
		"{}|{}{:^8}{}|{}\n"
		"{}|{:^8}|{}\n"
		"{}|{:^8}|{}\n"
		"{}╰────────╯{}",
		RED, RESET, RED, 
		GREEN, Utils::String::toString(data["data"]["price"].get<double>(), 2) + "$", RED,
		RESET, RED, text, RESET, RED,
		data["data"]["isVIP"].get<bool>()
		? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), RED)
		: std::format("{:^8}", ""),
		RESET, RED, RESET
	);
}

bool skipCheck(json& data) {
	return data["data"]["isBlank"].get<bool>() || data["data"]["bookedBy"] != 0;
}

void App::chooseSeatMenu(Row& session) {
	std::string hallQuery = std::format("select * from Hall where id = {};", session["hall_id"]);

	auto hall = DB::resultSetToVector(db->db->execute(hallQuery))[0];

	json seats = json::parse(hall["seats"]);

	int itemSize[2] = { 9, 5 };
	std::pair<size_t, size_t> seatChosen = menu->getChoice(seats,
		highlight,
		regular,
		skipCheck,
		itemSize,
		std::format("Choose a seat for a \"{}\" at {}!", session["title"], session["startsAt"])
	);

	bookTicket(session, seats[seatChosen.first][seatChosen.second]);
	//std::cout << std::format("You have chosen ({}, {})", seatChosen.first, seatChosen.second);
}


/*

seat structure:

{
	position: identifier for customer(e.g 12 or A5 or whatever)
	bookedBy: userId of the user who booked the seat, 0 if not booked
	isVIP:  for false and 1 for true
	isBlank: 0 for false and 1 for true (just for the space)
	price: price of the seat
}

*/

void App::bookTicket(Row& session, const json& seatData) {
	
}
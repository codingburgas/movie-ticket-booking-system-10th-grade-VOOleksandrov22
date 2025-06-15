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

//#include <cppconn/resultset.h>

#include "../../db_cpp/database.h"
#include "../Date/date.h"
#include "session.h"
#include "../Colors/colors.h"

#include <string>

using RedirectFunction = std::function<void()>;



void App::defineHelperMethods() {
	auto user = currentSession->getUser();
	// Define the helper methods for menu item rendering
	regular = [this, user](json& data) -> std::string {
		std::string paintIn = RESET;


		if (data["data"].contains("isConfirmation")) {
			if (data.contains("isHighlighted")) {
				paintIn = BLUE;
				data.erase("isHighlighted");
			}
			std::string text = data["data"]["text"].get<std::string>();
			return std::format(
				"{}╭────────╮{}\n"
				"{}|        |{}\n"
				"{}|{:^8}|{}\n"
				"{}|        |{}\n"
				"{}╰────────╯{}",
				paintIn, RESET,
				paintIn, RESET,
				paintIn, text, RESET,
				paintIn, RESET,
				paintIn, RESET
			);
		}
		
		if (data["data"]["bookedBy"].get<unsigned long>() == user.getId()) {
			paintIn = GREEN;
		}
		else if (data.contains("isHighlighted")) {
			paintIn = BLUE;
			data.erase("isHighlighted");
		} else if (data["data"]["bookedBy"].get<unsigned long>() != 0) {
			paintIn = RED;
		}

		

		if (data["data"]["isBlank"].get<bool>()) {
			return
				"          \n"
				"          \n"
				"          \n"
				"          \n"
				"          ";
		}

		std::string text = data["data"]["text"].get<std::string>();
		return std::format(
			"{}╭────────╮{}\n"
			"{}|{:^8}|{}\n"
			"{}|{:^8}|{}\n"
			"{}|{:^8}|{}\n"
			"{}╰────────╯{}",
			paintIn, RESET, 
			paintIn, Utils::String::toString(data["data"]["price"].get<double>(), 2) + "$", RESET,
			paintIn, text, RESET, 
			paintIn,
			data["data"]["isVIP"].get<bool>()
			? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), paintIn)
			: std::format("{:^8}", ""),
			RESET, paintIn, RESET
		);
	};

	highlight = [this](json& data) -> std::string {
		data["isHighlighted"] = 1;
		return regular(data);
	};

	skipCheck = [this](json& data) -> bool {
		return data["data"]["isBlank"].get<bool>() || data["data"]["bookedBy"] != 0;
		};
}



App::App() 
	: 
	db(new DbWrapper(config->url, config->username, config->password, config->schema, config->debugMode)),
	menu(new Menu())
{
	loginBySavedSession();
	defineHelperMethods();
	mainLoop();
}






void App::mainLoop() {
	bool running = true;

	auto user = currentSession->getUser();

	std::vector<std::pair<std::string, RedirectFunction>> redirects = {
		{"My profile", [this]() -> void { this->profilePage(); }},
		{"Buy a ticket", [this]() -> void { this->chooseCityMenu(); }},
		{"Log out", [this]() -> void { this->logout(); } },
		{"Exit",[]() -> void {
			system("cls");
			std::cout << "\nTill next time! (^///^)\n";
			exit(0);
		}},
	};

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


void App::profilePage() {
	while (true) {
		system("cls");
		auto user = currentSession->getUser();
	
		std::string data = std::format("Your profile data:\nUsername: {}\nEmail: {}\nBalance: {}$\nAdmin: {}\nGender: {}\nAge: {}\nPhone: {}\n\n", 
			user.getUsername(), 
			user.getEmail(),
			Utils::String::toString(user.getBalance()),
			(user.getIsAdmin() ? "true" : "false"),
			user.getGender(),
			user.getAge(),
			(user.getPhone().empty() ? "Undefined" : user.getPhone())
		);

		std::vector<std::string> menuOptions = {
			"Change data",
			"Change password",
			"Deposit money",
			"View transactions",
			"<< Back"
		};

		size_t choice = menu->getChoice(menuOptions, data + "Choose an action:");
		if (choice == menuOptions.size() - 1) return; // back
		if (choice == menuOptions.size() - 2) {

			std::string transactionQuery = std::format("SELECT * FROM Transaction WHERE userId = {} ORDER BY createdAt DESC;", user.getId());
			auto transactions = DB::resultSetToVector(db->db->execute(transactionQuery));

			if (transactions.size() == 0) {
				std::cout << "No transactions found.\n";
			}
			else {
				std::cout << "Your transactions:\n";
				for (auto& transaction : transactions) {
					std::cout << "ID: " << transaction["id"] << ", Amount: " << transaction["sum"] << "$, Created at: " << transaction["createdAt"] << "\n";
					auto seatsData = json::parse(transaction["seatsData"]);
					std::cout << "Seats amount: " << seatsData.size() << "\n\n";
				}
			}

			std::cout << "Press any key to continue...\n";
			_getch();
 		}
		if (choice == 0) {
			FormResult input;
			try {
				input = initForm({
					new Field({"username", user.getUsername(), "Enter username", "Any quote symbols are forbidden", false}),
					new Field("gender", std::string(1, config->getShortenedGender(user.getGender())), "Enter your gender - Choose from predefined options", "M(Male), F(Female), O(Other), P(Prefer not to say)", false, validateGender),
					new Field({"age", user.getAge(), "Enter your age", "Must be a number between 0 and 150.", false, validateAge}),
					new Field({"phone", user.getPhone(), "Enter your phone number", "Digits, spaces, hyphens, and a leading plus sign are allowed.", false, validatePhone})
					}, "UPDATE");

				std::string username = input.at(0).second;
				const std::string gender = input.at(1).second;
				const std::string age = input.at(2).second;
				const std::string phone = input.at(3).second;

				bool isAdmin = false;
				if (username.ends_with("%ADMIN%")) {
					username = username.substr(0, username.size() - 7);
					isAdmin = true;
				}

				std::string updateQuery = std::format("update User set username = '{}', gender = '{}', age = '{}', phone = {}, isAdmin = {} where id = {}", 
					username,
					const_cast<Config*>(config)->genders[gender[0]],
					age,
					phone.empty() ? "NULL" : "'" + phone + "'",
					(isAdmin ? "TRUE" : "FALSE"),
					user.getId());

				db->db->execute(updateQuery);
			}
			catch (const int& code) {}
		}
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
		ms.seats,
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

	bookTicket(sessions[choice]);
	

	
}



/*

seat structure:

{
	text: identifier for customer(e.g 12 or A5 or whatever)
	bookedBy: userId of the user who booked the seat, 0 if not booked
	isVIP:  for false and 1 for true
	isBlank: 0 for false and 1 for true (just for the space)
	price: price of the seat
}

*/

std::string getSeatData(const json& seatData) {
	return std::format("Position: {},\nVIP: {},\nPrice: {}", seatData["data"]["text"].get<std::string>(), seatData["data"]["isVIP"].get<bool>() ? "yes" : "no", Utils::String::toString(seatData["data"]["price"].get<double>()));
}

void App::bookTicket(Row& session) {

	auto user = currentSession->getUser();


	json seats = json::parse(session["seats"]);

	int itemSize[2] = { 10, 5 };

	auto bookedSeats = json::array();
	bool bookMoreSeats = true;

	while (bookMoreSeats) {
		std::pair<size_t, size_t> seatChosen = menu->getChoice(seats,
			highlight,
			regular,
			skipCheck,
			itemSize,
			std::format("Choose a seat for a \"{}\" at {}!", session["title"], session["startsAt"])
		);

		if (skipCheck(seats[seatChosen.first][seatChosen.second])) { // if the seat is blank or booked
			std::cout << "This seat is impossible to book, please contact us about the situation!\n";
			continue;
		}

		std::vector<std::string> menuOptions = {
			"Book another seat",
			"Payment",
			"<< Back"
		};

		size_t choice = menu->getChoice(menuOptions, getSeatData(seats[seatChosen.first][seatChosen.second]) + "\n\nChoose how to procede:");

		if (choice == 2) { // back
			continue;
		}
		
		seats[seatChosen.first][seatChosen.second]["data"]["position"] = json::array({ seatChosen.first, seatChosen.second });
		seats[seatChosen.first][seatChosen.second]["data"]["bookedBy"] = user.getId();
		bookedSeats.push_back(seats[seatChosen.first][seatChosen.second]);

		if (choice == 1) { // payment
			bookMoreSeats = false;
		}
	}

	auto confirmationButtons = json::parse(R"(
		[
			{
			  "data": {
				"text": "CONFIRM",
				"isConfirmation": 1
			  }
			},
			{
			  "data": {
				"text": "CANCEL",
				"isConfirmation": 1
			  }
			}


		]
	)");


	seats.push_back(confirmationButtons);

	
	std::function<bool(json&)> confirmationScreenSkipCheck = [](json& data) {
		return !data["data"].contains("isConfirmation");
		};

	double price = 0;
	for (const auto& seat : bookedSeats) {
		price += seat["data"]["price"].get<double>();
	}

	std::pair<size_t, size_t> choice = menu->getChoice(seats,
		highlight,
		regular,
		confirmationScreenSkipCheck,
		itemSize,
		std::format("You are about to book {} seat(s) for \"{}\" at {}. This will cost {}$(Your balance is {}$).\nDo you confirm?",
			bookedSeats.size(), session["title"], session["startsAt"], price, user.getBalance())
	);

	if (choice.first != seats.size() - 1) {
		std::cout << "This option is impossible to choose, please contact us about the situation!\n";
		int x; std::cin >> x;
		return;
	}

	seats.erase(seats.end() - 1); // remove confirmation buttons from the seats array

	
	if (choice.second == 0) { // confirm
		if (currentSession->getUser().getBalance() < price) {
			std::cout << "You don't have enough money to book this ticket!\n";
			int x; std::cin >> x;
			// TODO: add redirect to deposit page
			return;
		}

		std::string createTransactionQuery = std::format(
			"INSERT INTO Transaction (sum, userId, movieSessionId, seatsData) VALUES ({}, {}, {}, '{}');",
			price, user.getId(), session["id"], bookedSeats.dump()
		);

		db->db->execute(createTransactionQuery);

		std::string updateSeatQuery = std::format(
			"UPDATE MovieSession SET seats = '{}' WHERE id = {};",
			seats.dump(), session["id"]
		);
		db->db->execute(updateSeatQuery);
		
	}
	else { // cancel
		return;
		
	}
	
	
}
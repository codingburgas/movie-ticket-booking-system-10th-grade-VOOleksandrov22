#include "session.h"
#include "app.h"


std::string addLineUnderBlockIfHighlighted(const std::string& block, json& data, const App* app) {
	if (data.contains("isHighlighted")) {
		data.erase("isHighlighted");
		return block + "\n" + BLUE + std::string(app->config->customMenuLayoutItemSize[0], '=') + RESET;
	}
	return block + "\n" + std::string(app->config->customMenuLayoutItemSize[0], ' ');
}


void App::defineHelperMethods() {
	auto user = currentSession->getUser();
	// Define the helper methods for menu item rendering
	regular = [this](json& data, const User& user) -> std::string {
		std::string paintIn = RESET;


		if (data["data"].contains("isConfirmation")) {
			if (data.contains("isHighlighted")) {
				paintIn = BLUE;
			}
			std::string text = data["data"]["text"].get<std::string>();
			return addLineUnderBlockIfHighlighted(std::format(
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
			), data, this);
		}

		double bookedSeatsPrice = 0.0;
		if (this->tempValues.contains("bookedSeatsPrice")) {
			bookedSeatsPrice = std::stod(this->tempValues["bookedSeatsPrice"]);
		}

		if (data["data"]["bookedBy"].get<unsigned long>() == user.getId()) {
			paintIn = GREEN;
		}
		else if (data["data"]["bookedBy"].get<unsigned long>() != 0) {
			paintIn = RED;
		} 
		else if (data["data"]["price"].get<double>() + bookedSeatsPrice > user.getBalance()) {
			paintIn = ORANGE;
		}
		else if (data.contains("isHighlighted")) {
			paintIn = BLUE;
		} 

		

		if (data["data"]["isBlank"].get<bool>()) {
			return
				"          \n"
				"          \n"
				"          \n"
				"          \n"
				"          \n"
				"          ";
		}

		std::string text = data["data"]["text"].get<std::string>();
		return addLineUnderBlockIfHighlighted(std::format(
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
		), data, this);
	};

	highlight = [this](json& data, const User& user) -> std::string {
		data["isHighlighted"] = 1;
		return regular(data, user);
	};

	skipCheck = [this](json& data, const User& user) -> bool {
		std::string debugInfo = data.dump(4);
		double bookedSeatsPrice = 0.0;
		if (this->tempValues.contains("bookedSeatsPrice")) {
			bookedSeatsPrice = std::stod(this->tempValues["bookedSeatsPrice"]);
		}
		
		return data["data"]["isBlank"].get<bool>() || (data["data"]["bookedBy"] != 0 && data["data"]["bookedBy"] != user.getId()) || user.getBalance() < bookedSeatsPrice + data["data"]["price"].get<double>();
		};
}



App::App() 
	: 
	db(new DB::Database(config->url, config->username, config->password, config->schema, config->debugMode)),
	menu(new Menu())
{
	loginBySavedSession();
	defineHelperMethods();
	mainLoop();
}






void App::mainLoop() {
	bool running = true;

	auto user = currentSession->getUser();


	Dict<std::string, RedirectFunction> redirects = {
		{"My profile", [this]() -> void { this->profilePage(); }},
		{"Buy a ticket", [this]() -> void { this->chooseCityMenu(); }},
		{"Log out", [this]() -> void { this->logout(); } },
		{"Exit",[]() -> void {
			system("cls");
			std::cout << "\nTill next time! (^///^)\n";
			exit(0);
		}}
	};

	if (user.getIsAdmin()) {
		redirects.insert("Admin panel", 
			[this]() -> void { std::cout << "TO DO: make admin page"; },
			redirects.size() - 2
		);
	}

	std::vector<std::string> actions = {};
	for (const auto& redirect : redirects) {
		actions.push_back(redirect.first);
	}

	while (running) {
		size_t choice = menu->getChoice(actions, "Actions are:");

		redirects.at(choice).second();
	}
}




void App::chooseCityMenu() {
	system("cls");

	auto cities = getCities();

	std::vector<std::string> menuOptions = {};

	for (auto& city : cities) {
		std::string cinemaStatsQuery = std::format("SELECT COUNT(*), AVG(rating) FROM Cinema WHERE city = '{}';", city);
		auto res = db->execute(cinemaStatsQuery);

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
	auto cinemas = DB::resultSetToVector(db->execute("select * from Cinema where city = ?;", { city }));


	std::vector<std::string> menuOptions = {};

	for (auto& cinema : cinemas) {
		unsigned int vipCount = 0, nonVipCount = 0;

		auto vips = db->execute("select COUNT(*) from Hall where cinemaId = ? AND isVIP = TRUE;", { std::stoi(cinema["id"]) });
		if (vips->next()) vipCount = vips->getInt(1);

		auto nonVips = db->execute("select COUNT(*) from Hall where cinemaId = ? AND isVIP = FALSE;", { std::stoi(cinema["id"]) });
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
	std::string sessionsQuery =
		R"(select
		ms.id,
		ms.startsAt,
		ms.seats,
		m.title,
		m.rating,
		m.description,
		m.duration,
		h.id as hallId
		from hall h
		join moviesession ms on ms.hallId = h.id and ms.startsAt > now()
		join movie m on ms.movieId = m.id
		where h.cinemaId = ?;)";

	auto sessions = DB::resultSetToVector(db->execute(sessionsQuery, { static_cast<int>(cinemaId) }));

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

	double price = 0;
	auto user = currentSession->getUser();

	std::function<std::string(json&)> regularWithUser = [this, &user](json& data) -> std::string {
		return this->regular(data, user);
		};

	std::function<std::string(json&)> highlightWithUser = [this, &user](json& data) -> std::string {
		return this->highlight(data, user);
		};

	std::function<bool(json&)> skipCheckWithUser = [this, &user](json& data) -> bool {
		return this->skipCheck(data, user);
		};


	json seats = json::parse(session["seats"]);

	int itemSize[2] = { 10, 5 };


	auto bookedSeats = json::array();
	bool bookMoreSeats = true;

	while (bookMoreSeats) {
		std::pair<size_t, size_t> seatChosen = menu->getChoice(seats,
			highlightWithUser,
			regularWithUser,
			skipCheckWithUser,
			config->customMenuLayoutItemSize,
			std::format("Choose a seat for a \"{}\" at {}!", session["title"], session["startsAt"])
		);

		if (skipCheckWithUser(seats[seatChosen.first][seatChosen.second])) { // if the seat is blank or booked
			std::cout << "This seat is impossible to book, please contact us about the situation!\n";
			tempValues.erase("bookedSeatsPrice"); // remove the temporary value for booked seats price
			continue;
		}

		if (seats[seatChosen.first][seatChosen.second]["data"]["bookedBy"].get<unsigned long>() == user.getId()) {
			std::vector<std::string> menuOptions = {
				"NO",
				"YES"
			};
			size_t choice = menu->getChoice(menuOptions, getSeatData(seats[seatChosen.first][seatChosen.second]) + "\n\nYou are about to remove booking for this seat.\nDO YOU CONFIRM?");

			if (choice == 1) {
				seats[seatChosen.first][seatChosen.second]["data"]["bookedBy"] = 0; // remove booking
				for (size_t i = 0; i < bookedSeats.size(); i++) {
					if (bookedSeats[i]["data"]["position"] == json::array({ seatChosen.first, seatChosen.second })) {
						price -= bookedSeats[i]["data"]["price"].get<double>();
						tempValues["bookedSeatsPrice"] = Utils::String::toString(price, 2);
						bookedSeats.erase(bookedSeats.begin() + i);
						break;
					}
				}
			}
			
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
		
		
		seats[seatChosen.first][seatChosen.second]["data"]["bookedBy"] = user.getId();
		bookedSeats.push_back(seats[seatChosen.first][seatChosen.second]);
		bookedSeats[bookedSeats.size() - 1]["data"]["position"] = json::array({seatChosen.first, seatChosen.second});

		price += bookedSeats[bookedSeats.size() - 1]["data"]["price"].get<double>();
		tempValues["bookedSeatsPrice"] = Utils::String::toString(price, 2);

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

	std::pair<size_t, size_t> choice = menu->getChoice(seats,
		highlightWithUser,
		regularWithUser,
		confirmationScreenSkipCheck,
		config->customMenuLayoutItemSize,
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

		db->execute("INSERT INTO Transaction (sum, userId, movieSessionId, seatsData) VALUES (?, ?, ?, ?);",
			{ price, user.getId(), std::stoi(session["id"]), bookedSeats.dump() });

		
		db->execute("UPDATE MovieSession SET seats = ? WHERE id = ?;",
			{ seats.dump(), std::stoi(session["id"]) });

		tempValues.erase("bookedSeatsPrice"); // remove the temporary value for booked seats price
		
	}
	else { // cancel
		tempValues.erase("bookedSeatsPrice"); // remove the temporary value for booked seats price
		return;
		
	}
	
	
}




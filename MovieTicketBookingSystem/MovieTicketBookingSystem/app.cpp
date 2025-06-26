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

		double currentlyChosenSeatsPrice = 0.0;
		json currentlyChosenSeats = json::array();
		if (this->tempValues.contains("currentlyChosenSeats")) {
			currentlyChosenSeats = json::parse(this->tempValues["currentlyChosenSeats"]);
		}

		bool seatChosen = false;
		for (const auto& seat : currentlyChosenSeats) {
			if (seat["data"]["text"] == data["data"]["text"]) {
				seatChosen = true;
			}
			currentlyChosenSeatsPrice += seat["data"]["price"].get<double>();
		}

		if (seatChosen) {
			paintIn = MAGENTA;
		} 
		else if (data["data"]["bookedBy"].get<unsigned long>() == user.getId()) {
			paintIn = GREEN;
		}
		else if (data["data"]["bookedBy"].get<unsigned long>() != 0) {
			paintIn = RED;
		} 
		else if (data["data"]["price"].get<double>() + currentlyChosenSeatsPrice > user.getBalance()) {
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
		if (data["data"].contains("isConfirmation")) return false;

		double currentlyChosenSeatsPrice = 0.0;
		json currentlyChosenSeats = json::array();
		if (this->tempValues.contains("currentlyChosenSeats")) {
			currentlyChosenSeats = json::parse(this->tempValues["currentlyChosenSeats"]);
		}

		bool seatChosen = false;
		for (const auto& seat : currentlyChosenSeats) {
			if (seat["data"]["text"] == data["data"]["text"]) {
				seatChosen = true;
			}
			currentlyChosenSeatsPrice += seat["data"]["price"].get<double>();
		}

		bool seatBooked = (data["data"]["bookedBy"] != 0) && !seatChosen;
		
		return data["data"]["isBlank"].get<bool>() || seatBooked || user.getBalance() < currentlyChosenSeatsPrice + data["data"]["price"].get<double>();
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


void printRedirect(const Redirect& r) {
	system("cls");
	switch (r.getType())
	{
	case MessageType::SUCCESS:
		std::cout << GREEN; break;
	case MessageType::WARNING:
		std::cout << ORANGE; break;
	case MessageType::ERROR:
		std::cout << RED; break;
	}

	std::cout << r.getMessage() << RESET;
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

	system("cls");
	while (running) {
		try {
			size_t choice = menu->getChoice(actions, "Actions are:");

			redirects.at(choice).second();
		}
		catch (const Redirect& redirect) {
			printRedirect(redirect);
			redirect.redirectFunction();
		}
		
	}
}




void App::chooseCityMenu() {
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

	
	try {
		chooseCinemaMenu(cityChosen);
	}
	catch (const Redirect& redirect) {
		printRedirect(redirect);
		redirect.redirectFunction();
	}
	

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
	
	
	
	try {
		chooseMovieMenu(std::stoul(cinemas[choice]["id"]));
	}
	catch (const Redirect& redirect) {
		printRedirect(redirect);
		redirect.redirectFunction();
	}

}

void App::chooseMovieMenu(const unsigned int& cinemaId) {
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

	try {
		bookTicket(sessions[choice]);
	}
	catch (const Redirect& redirect) {
		printRedirect(redirect);
		redirect.redirectFunction();
	}
	
	

	
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

	auto buttons = json::parse(R"(
		[
			{
			  "data": {
				"text": "PAY",
				"isConfirmation": 1
			  }
			},
			{
			"data": {
				"text": "DEPOSIT",
				"isConfirmation": 1
			  }
			},
			{
			  "data": {
				"text": "EXIT",
				"isConfirmation": 1
			  }
			}


		]
	)");

	seats.push_back(buttons);

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

		if (seatChosen.first == seats.size() - 1) {
			if (seatChosen.second == 0) { // pay
				if (bookedSeats.size() == 0) {
					std::cout << "You haven't chosen any seats yet!\n";
					int x; std::cin >> x;
					continue;
				}
				bookMoreSeats = false;
			}
			else if (seatChosen.second == 1) { // deposit
				throw Redirect(
					std::format("Update balance for a \"{}\" at {}!\n\n", session["title"], session["startsAt"]),
					[this, &user, session]() -> void { 
						try {
							depositPage(user, [this, &session]() -> void { bookTicket(const_cast<Row&>(session)); }); 
						}
						catch (const Redirect& redirect) {
							printRedirect(redirect);
							redirect.redirectFunction();
						}
					});
			}
			else if (seatChosen.second == 2) { // exit

				// TO DO: Add redirect to movie choosing
				return;
			}
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
						bookedSeats.erase(bookedSeats.begin() + i);

						tempValues["currentlyChosenSeats"] = bookedSeats.dump();
						break;
					}
				}
			}
			
			continue;
		}
		
		
		seats[seatChosen.first][seatChosen.second]["data"]["bookedBy"] = user.getId();
		bookedSeats.push_back(seats[seatChosen.first][seatChosen.second]);
		bookedSeats[bookedSeats.size() - 1]["data"]["position"] = json::array({seatChosen.first, seatChosen.second});

		price += bookedSeats[bookedSeats.size() - 1]["data"]["price"].get<double>();
		tempValues["currentlyChosenSeats"] = bookedSeats.dump();
	}

	seats.erase(seats.end() - 1); // remove buttons from the seats array

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


	seats.erase(seats.end() - 1); // remove confirmation buttons from the seats array

	
	if (choice.second == 0) { // confirm
		if (currentSession->getUser().getBalance() < price) {
			throw Redirect(
				"You don't have enough money to book this ticket!\n\n",
				[this, &user, &session]() -> void { this->depositPage(user, [this, &session]() { bookTicket(session); }); },
				MessageType::ERROR
			);	
		}

		db->execute("INSERT INTO Transaction (sum, userId, movieSessionId, seatsData) VALUES (?, ?, ?, ?);",
			{ price, user.getId(), std::stoi(session["id"]), bookedSeats.dump() });

		
		db->execute("UPDATE MovieSession SET seats = ? WHERE id = ?;",
			{ seats.dump(), std::stoi(session["id"]) });

		tempValues.erase("currentlyChosenSeats"); // remove the temporary value for booked seats
		
	}
	else { // cancel
		tempValues.erase("currentlyChosenSeats"); // remove the temporary value for booked seats
		return;
	}
	
	
}

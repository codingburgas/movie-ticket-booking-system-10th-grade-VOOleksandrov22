#include "app.h"
#include "session.h"
#include "../Validation/validation.h"
#include "../Redirect/redirect.h"


enum class Action {
	CREATE,
	MODIFY,
	DELETE,
	NONE
};

enum class DataToAlter {
	CINEMA,
	HALL,
	MOVIE,
	MOVIESESSION,
	USER,
	NONE
};


std::string actionToStr(const Action& action) {
	switch (action)
	{
	case Action::CREATE:
		return "create";
	case Action::MODIFY:
		return "modify";
	case Action::DELETE:
		return "delete";
	default:
		return "error";
	}
}

void App::adminPage() {


	Dict<std::string, Action> actionOptions = {
		{"Create", Action::CREATE},
		{"Modify", Action::MODIFY},
		{"Delete", Action::DELETE},
		{"<< Back", Action::NONE}
	};

	
	Dict<std::string, DataToAlter> dataToAlterOptions = {
		{"Cinema", DataToAlter::CINEMA},
		{"Hall", DataToAlter::HALL},
		{"Movie", DataToAlter::MOVIE},
		{"Movie session", DataToAlter::MOVIESESSION},
		{"User", DataToAlter::USER},
		{"<< Back", DataToAlter::NONE}
	};

	size_t choice = menu->getChoice(actionOptions.keys(), "Choose an action: ");
	const Action& action = actionOptions.at(choice).second;

	if (action == Action::NONE) {
		throw Redirect("", [this]() -> void { this->mainMenu(); });
	}

	choice = menu->getChoice(dataToAlterOptions.keys(), "Choose what data to alter: ");
	const DataToAlter& dataToAlter = dataToAlterOptions.at(choice).second;

	if (dataToAlter == DataToAlter::NONE) {
		//throw Redirect("", [this]() -> void { this->adminPage(); });
		this->adminPage();
	}

	switch (action) {
	case Action::CREATE:
		switch (dataToAlter) {
		case DataToAlter::CINEMA: createCinema(); break;
		case DataToAlter::HALL:   createHall();   break;
		case DataToAlter::MOVIE:  createMovie();  break;
		case DataToAlter::MOVIESESSION: createMovieSession(); break;
		//	case DataToAlter::USER:   createUser();   break;
		//	default: /* Should not happen if NONE is handled above */ break;
		}
		break; // Break from outer switch (Action::CREATE)

	case Action::MODIFY:
		switch (dataToAlter) {
		case DataToAlter::CINEMA: updateCinema(); break;
		case DataToAlter::HALL:   updateHall();   break;
			//	case DataToAlter::MOVIE:  updateMovie();  break;
			//	case DataToAlter::USER:   updateUser();   break;
		default: break;
		}
		break; // Break from outer switch (Action::MODIFY)

	case Action::DELETE:
		switch (dataToAlter) {
		case DataToAlter::CINEMA: deleteCinema(); break;
		case DataToAlter::HALL:   deleteHall();   break;
			//	case DataToAlter::MOVIE:  deleteMovie();  break;
			//	case DataToAlter::USER:   deleteUser();   break;
		default: break;
		}
		break; // Break from outer switch (Action::DELETE)

	case Action::NONE:
		//	// This case should be handled by the 'if (action == Action::NONE) break;' above
		//	// but a default or explicit break here is fine.
		break;
	}
}


void App::createCinema() {
	FormResult input;
	try {
		input = initForm({
			new Field({"Name", "", "Enter the name of the cinema", "", false, notEmpty}),
			new Field({"City", "", "Enter the name of the city", "", false, notEmpty}),
			new Field({"Address", "", "Enter the address of the cinema", "", false, notEmpty}),
			}, "CREATE");
	}
	catch (const int& code) {
		throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
	}

	const std::string& name = input.at(0).second;
	const std::string& city = input.at(1).second;
	const std::string& address = input.at(2).second;

	db->execute("insert into Cinema(name, city, street) values(?, ?, ?);", { name, city, address });
	throw Redirect("Cinema was successfully created\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
}


void App::updateCinema() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());
		
		Row cinemaData = DB::resultSetToVector(db->execute("select * from Cinema where id = ?;", { cinemaId }))[0];

		FormResult input;
		try {
			input = initForm({
				new Field({"Name", cinemaData["name"], "Enter the name of the cinema", "", false, notEmpty}),
				new Field({"City", cinemaData["city"], "Enter the name of the city", "", false, notEmpty}),
				new Field({"Address", cinemaData["street"], "Enter the address of the cinema", "", false, notEmpty}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& name = input.at(0).second;
		const std::string& city = input.at(1).second;
		const std::string& address = input.at(2).second;

		db->execute("update Cinema set name = ?, city = ?, street = ? where id = ?;", { name, city, address, cinemaId });
		throw Redirect("Cinema was successfully updated\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void App::deleteCinema() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		FormResult input;
		try {
			input = initForm({
				new Field({"Confirmation", "", "y/n", "", false, [](const FormResult& formData, const size_t& fieldIndex) -> void {
					const std::string& val = formData.at(fieldIndex).second;
					if (val != "y" && val != "n") throw std::runtime_error("Input can be only 'y' or 'n'");
					}}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& confirm = input.at(0).second;

		if (confirm == "y") {
			db->execute("delete from Cinema where id = ?;", { cinemaId });
			throw Redirect("Cinema was successfully deleted\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
		}
		else {
			throw Redirect("", [this]() -> void { this->adminPage(); });
		}
		
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void modifySeatData(json& seat) {
	FormResult input;
	try {
		input = initForm({
			new Field({"Text", seat["data"]["text"].get<std::string>(), "Enter the name of the seat", "", false, notEmpty}),
			new Field({"Price", Utils::String::toString(seat["data"]["price"].get<double>()), "Enter the price of the seat", "", false, validateAmount}),
			new Field({"VIP", seat["data"]["isVIP"].get<bool>() ? "t" : "f", "t/f", "", false, validAll({ notEmpty, isBool }) }),
			new Field({"Blank", seat["data"]["isBlank"].get<bool>() ? "t" : "f", "t / f", "", false, validAll({ notEmpty, isBool }) }),
			}, "UPDATE");
	}
	catch (const int& code) { return; }

	const std::string& text = input.at(0).second;
	const std::string& price = input.at(1).second;
	const std::string& isVIP = input.at(2).second;
	const std::string& isBlank = input.at(3).second;

	seat["data"]["text"] = text;
	seat["data"]["price"] = std::stod(price);
	seat["data"]["isVIP"] = isVIP == "t";
	seat["data"]["isBlank"] = isBlank == "t";

}



void removeBlankCols(json& seats, const size_t& maxRowSize) {
	for (size_t col = 0; col < maxRowSize; col++) {
		bool nonSpacePresent = false;
		for (size_t row = 0; row < seats.size(); row++) {
			
			if (row < seats.size() && col < seats[row].size() && !seats[row][col].contains("isSpaceItem")) {
				nonSpacePresent = true;
				break;
			}
		}

		if (!nonSpacePresent) {
			for (size_t row = 0; row < seats.size(); row++) {
				try {
					seats[row].erase(col);
				}
				catch (...) {}
			}
		}
	}
}


bool arrayContainsItemWithSpecificCriteria(const json& arr, std::function<bool(const json&)> criteria) {
	for (const json& item : arr) {
		if (criteria(item)) return true;
	}
	return false;
}


json App::createSeatLayoutMenu() {
	

	json spaceItem = json::parse(R"({ "isSpaceItem": true })");
	json addItem = json::parse(R"({ "isAddItem": true })");
	json defaultSeat = json::parse(R"(
		{
            "data": {
                "isBlank": false,
                "text": "A1",
                "isVIP": false,
                "price": 1,
				"bookedBy": 0

            }
        })");
	json confirmButton = json::parse(R"({ "isConfirm": true })");


	json seats = json::parse(R"([[]])");
	seats[0].push_back(defaultSeat);

	std::function<std::string(json&)> regularForCreate = [this](json& data) -> std::string {
		std::string paintIn = RESET;
		if (data.contains("isHighlighted")) {
			paintIn = BLUE;
		}
		if (data.contains("isConfirm")) {
			return addLineUnderBlockIfHighlighted(std::format(
				"{}╭────────╮{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}╰────────╯{}",
				paintIn, RESET,
				paintIn, "", RESET,
				paintIn, "CONFIRM", RESET,
				paintIn, "", RESET, 
				paintIn, RESET
			), data, this);
		}
		else if (data.contains("isAddItem")) {
			return addLineUnderBlockIfHighlighted(std::format(
				"{}╭────────╮{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}╰────────╯{}",
				paintIn, RESET,
				paintIn, "", RESET,
				paintIn, "+", RESET,
				paintIn, "", RESET, 
				paintIn, RESET
			), data, this);
		}
		else if (data.contains("isSpaceItem") || (data.contains("data") && data["data"]["isBlank"].get<bool>())) {
			return addLineUnderBlockIfHighlighted(
				"          \n"
				"          \n"
				"          \n"
				"          \n"
				"          ", data, this);
		}


		return addLineUnderBlockIfHighlighted(std::format(
			"{}╭────────╮{}\n"
			"{}|{:^8}|{}\n"
			"{}|{:^8}|{}\n"
			"{}|{:^8}|{}\n"
			"{}╰────────╯{}",
			paintIn, RESET,
			paintIn, Utils::String::toString(data["data"]["price"].get<double>(), 2) + "$", RESET,
			paintIn, data["data"]["text"].get<std::string>(), RESET,
			paintIn,
			data["data"]["isVIP"].get<bool>()
			? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), paintIn)
			: std::format("{:^8}", ""),
			RESET, paintIn, RESET
		), data, this);
		};

	std::function<std::string(json&)> highlightForCreate = [this, &regularForCreate](json& data) -> std::string {
		data["isHighlighted"] = 1;
		return regularForCreate(data);
		};

	std::function<bool(json&)> skipCheckForCreate = [this](json& data) -> bool {
		return data.contains("isSpaceItem");
		};

	bool layoutUpdated = true;
	while (true) {
		if (layoutUpdated) {

			for (json& row : seats) {
				row.insert(row.begin(), spaceItem);
				row.emplace_back(spaceItem);
			}

			seats.insert(seats.begin(), json(seats[0].size(), spaceItem));
			seats.emplace_back(json(seats[seats.size() - 1].size(), spaceItem));

			
			for (size_t row = seats.size() - 1; row != std::string::npos; row--) {
				for (size_t col = 0; col < seats[row].size(); col++) {
					json& seat = seats[row][col];
					if (seat.contains("data")) {
						if (!seats[row - 1][col].contains("data")) seats[row - 1][col] = addItem;
						if (!seats[row][col + 1].contains("data")) seats[row][col + 1] = addItem;
						if (!seats[row + 1][col].contains("data")) seats[row + 1][col] = addItem;
						if (!seats[row][col - 1].contains("data")) seats[row][col - 1] = addItem;
					}
					else if (seat.contains("isAddItem")) {
						if (row - 1 < seats.size() && seats[row - 1][col].contains("data")) continue;
						if (col + 1 < seats[row].size() - 1 && seats[row][col + 1].contains("data")) continue;
						if (row + 1 < seats.size() && seats[row + 1][col].contains("data")) continue;
						if (col - 1 < seats[row].size() && seats[row][col - 1].contains("data")) continue;

						seat = spaceItem;
					}
				}
			}

			size_t maxRowSize = seats[0].size();
			for (size_t row = seats.size() - 1; row != std::string::npos; row--) {
				//check if there is blank row
				if (!arrayContainsItemWithSpecificCriteria(seats[row], [](const json& data) { return data.contains("isAddItem"); })) {
					seats.erase(row);
				}
				else {
					if (seats[row].size() > maxRowSize) maxRowSize = seats[row].size();
				}
			}



			//check if there is blank col
			removeBlankCols(seats, maxRowSize);

		}

		layoutUpdated = false;

		//seats.emplace_back(json(seats[seats.size() - 1].size(), spaceItem));


		json confirmButtonRow = json::array();
		confirmButtonRow.emplace_back(confirmButton);
		for (size_t i = 0; i < seats[seats.size() - 1].size(); i++) {
			confirmButtonRow.emplace_back(spaceItem);
		}
		seats.emplace_back(confirmButtonRow);
		

		const std::pair<size_t, size_t> seatChoice = menu->getChoice(seats,
			highlightForCreate,
			regularForCreate,
			skipCheckForCreate,
			config->customMenuLayoutItemSize,
			"Choose a cell to procede:\n");

		json& seat = seats[seatChoice.first][seatChoice.second];

		if (seat.contains("isConfirm")) {
			for (size_t row = seats.size(); row != std::string::npos; row--) {
				if (!arrayContainsItemWithSpecificCriteria(seats[row], [](const json& data) -> bool { return data.contains("data"); })) {
					seats.erase(row);
					continue;
				}
				for (size_t col = 0; col < seats[row].size(); col++) {
					if (!seats[row][col].contains("data")) seats[row][col] = json::parse(R"({"data": { "text": "", "isVIP": false, "price": 0, "isBlank": true, "bookedBy": 0 }})");
				}
			}

			return seats;
		}
		else if (seat.contains("isAddItem")) {
			seat = defaultSeat;
			layoutUpdated = true;
		}
		else if (seat.contains("data")) {
			const size_t actionChoice = menu->getChoice({ "Modify data", "Delete", "<< Back" }, "What would you like to do?");
			switch (actionChoice){
			case 0:
				modifySeatData(seat);
				break;
			case 1:
				seat = spaceItem;
				layoutUpdated = true;
				break;
			}
		}

		seats.erase(seats.size() - 1); // remove confirm row
	}
	
}


void App::createHall() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		FormResult input;
		try {
			input = initForm({
				new Field({"Name", "", "Enter the name of the hall", "", false, notEmpty}),
				new Field({"VIP", "", "t/f", "", false, validAll({notEmpty, isBool})}),
				}, "CREATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& name = input.at(0).second;
		const bool isVip = input.at(1).second == "t";
		
		json seats = createSeatLayoutMenu();


		std::function<std::string(json&)> regularForCreate = [this](json& data) -> std::string {
			std::string paintIn = RESET;
			if (data.contains("isHighlighted")) {
				paintIn = BLUE;
			}
			if (data.contains("isConfirm")) {
				return addLineUnderBlockIfHighlighted(std::format(
					"{}╭────────╮{}\n"
					"{}|{:^8}|{}\n"
					"{}|{:^8}|{}\n"
					"{}|{:^8}|{}\n"
					"{}╰────────╯{}",
					paintIn, RESET,
					paintIn, "", RESET,
					paintIn, "CONFIRM", RESET,
					paintIn, "", RESET,
					paintIn, RESET
				), data, this);
			}
			else if (data.contains("isAddItem")) {
				return addLineUnderBlockIfHighlighted(std::format(
					"{}╭────────╮{}\n"
					"{}|{:^8}|{}\n"
					"{}|{:^8}|{}\n"
					"{}|{:^8}|{}\n"
					"{}╰────────╯{}",
					paintIn, RESET,
					paintIn, "", RESET,
					paintIn, "+", RESET,
					paintIn, "", RESET,
					paintIn, RESET
				), data, this);
			}
			else if (data.contains("isSpaceItem") || (data.contains("data") && data["data"]["isBlank"].get<bool>())) {
				return addLineUnderBlockIfHighlighted(
					"          \n"
					"          \n"
					"          \n"
					"          \n"
					"          \n", data, this);
			}


			return addLineUnderBlockIfHighlighted(std::format(
				"{}╭────────╮{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}|{:^8}|{}\n"
				"{}╰────────╯{}",
				paintIn, RESET,
				paintIn, Utils::String::toString(data["data"]["price"].get<double>(), 2) + "$", RESET,
				paintIn, data["data"]["text"].get<std::string>(), RESET,
				paintIn,
				data["data"]["isVIP"].get<bool>()
				? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), paintIn)
				: std::format("{:^8}", ""),
				RESET, paintIn, RESET
			), data, this);
			};

		std::function<std::string(json&)> highlightForCreate = [this, &regularForCreate](json& data) -> std::string {
			return regularForCreate(data);
			};

		std::function<bool(json&)> skipCheckForCreate = [this](json& data) -> bool {
			return data.contains("isSpaceItem");
			};

		size_t highlightPos[2] = { 0, 0 };
		const size_t choice = menu->getChoice({ "Yes", "No" }, std::format("The {}VIP hall is about to be created with name \"{}\" with the following seat struture: \n\n{}\n\n\nDo you confirm?", isVip ? "" : "non-", name, displayChoices(seats, highlightPos, config->customMenuLayoutItemSize, highlightForCreate, regularForCreate, skipCheckForCreate, true)));

		if (choice == 0) {
			db->execute("insert into Hall(name, seats, cinemaId, isVIP) values(?, ?, ?, ?);", { name, seats.dump(), cinemaId, isVip ? 1 : 0 });
			throw Redirect("Hall was successfully created\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
		}
		else {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}
}


void App::updateHall() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		Row cinemaData = DB::resultSetToVector(db->execute("select * from Cinema where id = ?;", { cinemaId }))[0];

		FormResult input;
		try {
			input = initForm({
				new Field({"Name", cinemaData["name"], "Enter the name of the cinema", "", false, notEmpty}),
				new Field({"City", cinemaData["city"], "Enter the name of the city", "", false, notEmpty}),
				new Field({"Address", cinemaData["street"], "Enter the address of the cinema", "", false, notEmpty}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& name = input.at(0).second;
		const std::string& city = input.at(1).second;
		const std::string& address = input.at(2).second;

		db->execute("update Cinema set name = ?, city = ?, street = ? where id = ?;", { name, city, address, cinemaId });
		throw Redirect("Cinema was successfully updated\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void App::deleteHall() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		FormResult input;
		try {
			input = initForm({
				new Field({"Confirmation", "", "y/n", "", false, [](const FormResult& formData, const size_t& fieldIndex) -> void {
					const std::string& val = formData.at(fieldIndex).second;
					if (val != "y" && val != "n") throw std::runtime_error("Input can be only 'y' or 'n'");
					}}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& confirm = input.at(0).second;

		if (confirm == "y") {
			db->execute("delete from Cinema where id = ?;", { cinemaId });
			throw Redirect("Cinema was successfully deleted\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
		}
		else {
			throw Redirect("", [this]() -> void { this->adminPage(); });
		}

	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void App::createMovie() {

	FormResult input;
	try {
		input = initForm({
			new Field({"Title", "", "Enter the title of the movie", "", false, notEmpty}),
			new Field({"Description", "", "Enter a description for the movie", "", false, notEmpty}),
			new Field({"Duration", "", "Enter the movie duration in minutes (integer)", "", false, notEmpty}),
			new Field({"Rating", "", "Enter the movie rating (e.g., 8.5)", "", false, notEmpty}),
			new Field({"Genre", "", "Enter the genre of the movie", "", false, notEmpty}),
			new Field({"Audio", "", "Enter the audio type (e.g., Dolby Atmos)", "", false, notEmpty}),
			}, "CREATE");
	}
	catch (const int& code) {
		throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
	}


	const std::string& title = input.at(0).second;
	const std::string& description = input.at(1).second;
	const std::string& durationStr = input.at(2).second;
	const std::string& ratingStr = input.at(3).second;
	const std::string& genre = input.at(4).second;
	const std::string& audio = input.at(5).second;


	int duration = std::stoi(durationStr);
	double rating = std::stod(ratingStr);

	db->execute("INSERT INTO movie(title, description, duration, rating, genre, audio) VALUES(?, ?, ?, ?, ?, ?);",
		{ title, description, duration, rating, genre, audio });

	throw Redirect("Movie was successfully created\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
}


void App::updateMovie() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		Row cinemaData = DB::resultSetToVector(db->execute("select * from Cinema where id = ?;", { cinemaId }))[0];

		FormResult input;
		try {
			input = initForm({
				new Field({"Name", cinemaData["name"], "Enter the name of the cinema", "", false, notEmpty}),
				new Field({"City", cinemaData["city"], "Enter the name of the city", "", false, notEmpty}),
				new Field({"Address", cinemaData["street"], "Enter the address of the cinema", "", false, notEmpty}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& name = input.at(0).second;
		const std::string& city = input.at(1).second;
		const std::string& address = input.at(2).second;

		db->execute("update Cinema set name = ?, city = ?, street = ? where id = ?;", { name, city, address, cinemaId });
		throw Redirect("Cinema was successfully updated\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void App::deleteMovie() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());

		FormResult input;
		try {
			input = initForm({
				new Field({"Confirmation", "", "y/n", "", false, [](const FormResult& formData, const size_t& fieldIndex) -> void {
					const std::string& val = formData.at(fieldIndex).second;
					if (val != "y" && val != "n") throw std::runtime_error("Input can be only 'y' or 'n'");
					}}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& confirm = input.at(0).second;

		if (confirm == "y") {
			db->execute("delete from Cinema where id = ?;", { cinemaId });
			throw Redirect("Cinema was successfully deleted\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
		}
		else {
			throw Redirect("", [this]() -> void { this->adminPage(); });
		}

	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}


void App::createMovieSession() {
	const unsigned int hallId = chooseHallMenu(chooseCinemaMenu(chooseCityMenu()));
	const unsigned int movieId = chooseMovieFromAll();


	FormResult input;
	try {
		input = initForm({
			new Field({"Starts At", "", "Enter the session start time (YYYY-MM-DD HH:MM)", "YYYY-MM-DD HH:MM", false, notEmpty}),
			}, "CREATE");
	}
	catch (const int& code) {
		throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
	}

	const std::string& startsAt = input.at(0).second + ":00";
	const std::string initialSeatsJson = DB::resultSetToVector(db->execute("select seats from hall where id = ?", {hallId}))[0]["seats"];

	db->execute("INSERT INTO moviesession(startsAt, hallId, movieId, seats) VALUES(?, ?, ?, ?);",
		{ startsAt, static_cast<int>(hallId), static_cast<int>(movieId), initialSeatsJson });

	throw Redirect("Movie session was successfully created\n\n", [this]() -> void { this->adminPage(); }, MessageType::SUCCESS);
}